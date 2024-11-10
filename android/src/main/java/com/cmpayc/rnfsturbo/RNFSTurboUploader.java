/*
The MIT License (MIT)

Copyright (c) 2015 Johannes Lumpe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

Modified by Sergei Kazakov on 31.10.24.
*/

package com.cmpayc.rnfsturbo;

import android.os.AsyncTask;
import android.util.Log;
import android.webkit.MimeTypeMap;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.nio.channels.Channels;
import java.nio.channels.FileChannel;
import java.nio.channels.WritableByteChannel;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;

public class RNFSTurboUploader extends AsyncTask<RNFSTurboUploadParams, int[], RNFSTurboUploadResult> {
  private RNFSTurboUploadParams mParams;
  private RNFSTurboUploadResult res;
  private AtomicBoolean mAbort = new AtomicBoolean(false);

  @Override
  protected RNFSTurboUploadResult doInBackground(RNFSTurboUploadParams... uploadParams) {
    mParams = uploadParams[0];
    res = new RNFSTurboUploadResult();
    new Thread(new Runnable() {
      @Override
      public void run() {
        try {
          upload(mParams, res);
          mParams.onUploadComplete.onUploadComplete(res);
        } catch (Exception e) {
          res.exception = e;
          mParams.onUploadComplete.onUploadComplete(res);
        }
      }
    }).start();
    return res;
  }

  private void upload(RNFSTurboUploadParams params, RNFSTurboUploadResult result) throws Exception {
    HttpURLConnection connection = null;
    DataOutputStream request = null;
    String crlf = "\r\n";
    String twoHyphens = "--";
    String boundary = "*****";
    String tail = crlf + twoHyphens + boundary + twoHyphens + crlf;
    String metaData = "", stringData = "";
    String[] fileHeader;
    int statusCode, byteSentTotal;
    int fileCount = 0;
    long totalFileLength = 0;
    BufferedInputStream responseStream = null;
    BufferedReader responseStreamReader = null;
    String name, filename, filepath, filetype;
    try {
      Object[] files = new Object[params.filesNum];
      boolean binaryStreamOnly = params.binaryStreamOnly;

      connection = (HttpURLConnection) params.src.openConnection();
      connection.setDoOutput(true);
      connection.setRequestMethod(params.method);
      if (!binaryStreamOnly) {
        connection.setRequestProperty("Content-Type", "multipart/form-data;boundary=" + boundary);
      }
      for (String key : params.headers.keySet()) {
        connection.setRequestProperty(key, params.headers.get(key));
      }

      for (String key : params.fields.keySet()) {
        metaData += twoHyphens + boundary + crlf + "Content-Disposition: form-data; name=\"" + key + "\"" + crlf + crlf + params.fields.get(key) +crlf;
      }
      stringData += metaData;
      fileHeader = new String[files.length];
      for ( int i = 0; i < params.files.length - 1; i += 4) {
        name = params.files[i];
        filename = params.files[i + 1];
        filepath = params.files[i + 2];
        filetype = params.files[i + 3];
        if (filetype.isEmpty()) {
          filetype = getMimeType(filepath);
        }
        File file = new File(filepath);
        long fileLength = file.length();
        totalFileLength += fileLength;
        if (!binaryStreamOnly) {
          String fileHeaderType = twoHyphens + boundary + crlf +
            "Content-Disposition: form-data; name=\"" + name + "\"; filename=\"" + filename + "\"" + crlf +
            "Content-Type: " + filetype + crlf;
          ;
          if (files.length - 1 == fileCount) {
            totalFileLength += tail.length();
          }

          String fileLengthHeader = "Content-length: " + fileLength + crlf;
          fileHeader[fileCount] = fileHeaderType + fileLengthHeader + crlf;
          stringData += fileHeaderType + fileLengthHeader + crlf;
        }
        fileCount++;
      }
      fileCount = 0;
      if (mParams.onUploadBegin != null) {
        mParams.onUploadBegin.onUploadBegin();
      }
      if (!binaryStreamOnly) {
        long requestLength = totalFileLength;
        requestLength += stringData.length() + files.length * crlf.length();
        connection.setRequestProperty("Content-length", "" +(int) requestLength);
        connection.setFixedLengthStreamingMode((int)requestLength);
      }
      connection.connect();

      request = new DataOutputStream(connection.getOutputStream());
      WritableByteChannel requestChannel = Channels.newChannel(request);

      if (!binaryStreamOnly) {
        request.writeBytes(metaData);
      }

      byteSentTotal = 0;

      for ( int i = 0; i < params.files.length - 1; i += 4) {
        filepath = params.files[i + 2];
        if (!binaryStreamOnly) {
          request.writeBytes(fileHeader[fileCount]);
        }

        File file = new File(filepath);

        long fileLength = file.length();
        long bufferSize = (long) Math.ceil(fileLength / 100.f);
        long bytesRead = 0;

        FileInputStream fileStream = new FileInputStream(file);
        FileChannel fileChannel = fileStream.getChannel();

        while (bytesRead < fileLength) {
          long transferredBytes = fileChannel.transferTo(bytesRead, bufferSize, requestChannel);
          bytesRead += transferredBytes;

          if (mParams.onUploadProgress != null) {
            byteSentTotal += transferredBytes;
            mParams.onUploadProgress.onUploadProgress((int) totalFileLength, byteSentTotal);
          }
        }

        if (!binaryStreamOnly) {
          request.writeBytes(crlf);
        }

        fileCount++;
        fileStream.close();
      }

      if (!binaryStreamOnly) {
          request.writeBytes(tail);
      }
      request.flush();
      request.close();

      responseStream = new BufferedInputStream(connection.getInputStream());
      responseStreamReader = new BufferedReader(new InputStreamReader(responseStream));
      Map<String, List<String>> headers = connection.getHeaderFields();

      Map<String, String> headersFlat = new HashMap<>();

      for (Map.Entry<String, List<String>> entry : headers.entrySet()) {
        String headerKey = entry.getKey();
        String valueKey = entry.getValue().get(0);

        if (headerKey != null && valueKey != null) {
          headersFlat.put(headerKey, valueKey);
        }
      }
      StringBuilder stringBuilder = new StringBuilder();
      String line = "";

      while ((line = responseStreamReader.readLine()) != null) {
        stringBuilder.append(line).append("\n");
      }

      String response = stringBuilder.toString();
      statusCode = connection.getResponseCode();
      res.headers = headersFlat;
      res.body = response;
      res.statusCode = statusCode;
    } finally {
      if (connection != null)
        connection.disconnect();
      if (request != null)
        request.close();
      if (responseStream != null)
        responseStream.close();
      if (responseStreamReader != null)
        responseStreamReader.close();
    }
  }

  protected String getMimeType(String path) {
    String type = null;
    String extension = MimeTypeMap.getFileExtensionFromUrl(path);
    if (extension != null) {
      type = MimeTypeMap.getSingleton().getMimeTypeFromExtension(extension.toLowerCase());
    }
    if (type == null) {
      type = "*/*";
    }
    return type;
  }

  protected void stop() {
    mAbort.set(true);
  }
}
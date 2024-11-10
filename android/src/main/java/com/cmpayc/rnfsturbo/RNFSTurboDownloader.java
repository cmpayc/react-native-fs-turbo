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

import java.io.FileOutputStream;
import java.io.BufferedInputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.net.HttpURLConnection;
import java.util.HashMap;
import java.util.Map;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

import android.util.Log;

import android.os.AsyncTask;

public class RNFSTurboDownloader extends AsyncTask<RNFSTurboDownloadParams, long[], RNFSTurboDownloadResult> {
  private RNFSTurboDownloadParams mParam;
  private AtomicBoolean mAbort = new AtomicBoolean(false);
  RNFSTurboDownloadResult res;

  protected RNFSTurboDownloadResult doInBackground(RNFSTurboDownloadParams... params) {
    mParam = params[0];
    res = new RNFSTurboDownloadResult();

    new Thread(new Runnable() {
      public void run() {
        try {
          download(mParam, res);
          mParam.onTaskCompleted.onTaskCompleted(res);
        } catch (Exception ex) {
          res.exception = ex;
          mParam.onTaskCompleted.onTaskCompleted(res);
        }
      }
    }).start();

    return res;
  }

  private void download(RNFSTurboDownloadParams param, RNFSTurboDownloadResult res) throws Exception {
    InputStream input = null;
    OutputStream output = null;
    HttpURLConnection connection = null;

    try {
      connection = (HttpURLConnection)param.src.openConnection();

      for (String key : param.headers.keySet()) {
        connection.setRequestProperty(key, param.headers.get(key));
      }

      connection.setConnectTimeout(param.connectionTimeout);
      connection.setReadTimeout(param.readTimeout);
      connection.connect();

      int statusCode = connection.getResponseCode();
      long lengthOfFile = getContentLength(connection);

      boolean isRedirect = (
        statusCode != HttpURLConnection.HTTP_OK &&
          (
            statusCode == HttpURLConnection.HTTP_MOVED_PERM ||
              statusCode == HttpURLConnection.HTTP_MOVED_TEMP ||
              statusCode == 307 ||
              statusCode == 308
          )
      );

      if (isRedirect) {
        String redirectURL = connection.getHeaderField("Location");
        connection.disconnect();

        connection = (HttpURLConnection) new URL(redirectURL).openConnection();
        connection.setConnectTimeout(5000);
        connection.connect();

        statusCode = connection.getResponseCode();
        lengthOfFile = getContentLength(connection);
      }
      if(statusCode >= 200 && statusCode < 300) {
        Map<String, List<String>> headers = connection.getHeaderFields();

        Map<String, String> headersFlat = new HashMap<>();

        for (Map.Entry<String, List<String>> entry : headers.entrySet()) {
          String headerKey = entry.getKey();
          String valueKey = entry.getValue().get(0);

          if (headerKey != null && valueKey != null) {
            headersFlat.put(headerKey, valueKey);
          }
        }

        if (mParam.onDownloadBegin != null) {
          mParam.onDownloadBegin.onDownloadBegin(statusCode, lengthOfFile, headersFlat);
        }

        input = new BufferedInputStream(connection.getInputStream(), 8 * 1024);
        output = new FileOutputStream(param.dest);

        byte data[] = new byte[8 * 1024];
        long total = 0;
        int count;
        double lastProgressValue = 0;
        long lastProgressEmitTimestamp = 0;
        boolean hasProgressCallback = mParam.onDownloadProgress != null;

        while ((count = input.read(data)) != -1) {
          if (mAbort.get()) throw new Exception("Download has been aborted");

          total += count;

          if (hasProgressCallback) {
            if (param.progressInterval > 0) {
              long timestamp = System.currentTimeMillis();
              if (timestamp - lastProgressEmitTimestamp > param.progressInterval) {
                lastProgressEmitTimestamp = timestamp;
                publishProgress(new long[]{lengthOfFile, total});
              }
            } else if (param.progressDivider <= 0) {
              publishProgress(new long[]{lengthOfFile, total});
            } else {
              double progress = Math.round(((double) total * 100) / lengthOfFile);
              if (progress % param.progressDivider == 0) {
                if ((progress != lastProgressValue) || (total == lengthOfFile)) {
                  Log.d("RNFSTurboDownloader", "EMIT: " + String.valueOf(progress) + ", TOTAL:" + String.valueOf(total));
                  lastProgressValue = progress;
                  publishProgress(new long[]{lengthOfFile, total});
                }
              }
            }
          }

          output.write(data, 0, count);
        }

        output.flush();
        res.bytesWritten = total;
      }
      res.statusCode = statusCode;
    } finally {
      if (output != null) output.close();
      if (input != null) input.close();
      if (connection != null) connection.disconnect();
    }
  }

  private long getContentLength(HttpURLConnection connection){
    if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.N) {
      return connection.getContentLengthLong();
    }
    return connection.getContentLength();
  }

  protected void stop() {
    mAbort.set(true);
  }

  @Override
  protected void onProgressUpdate(long[]... values) {
    super.onProgressUpdate(values);
    if (mParam.onDownloadProgress != null) {
      mParam.onDownloadProgress.onDownloadProgress(values[0][0], values[0][1]);
    }
  }

  protected void onPostExecute(Exception ex) {

  }
}

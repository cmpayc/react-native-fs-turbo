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

import java.net.URL;
import java.util.Map;

public class RNFSTurboUploadParams {
  public interface onUploadComplete{
    void onUploadComplete(RNFSTurboUploadResult res);
  }
  public interface onUploadProgress{
    void onUploadProgress(double totalBytesExpectedToSend, double totalBytesSent);
  }
  public interface onUploadBegin{
    void onUploadBegin();
  }
  public URL src;
  public String[] files;
  public int filesNum;
  public boolean binaryStreamOnly;
  public String name;
  public Map<String, String> headers;
  public Map<String, String> fields;
  public String method;
  public onUploadComplete onUploadComplete;
  public onUploadProgress onUploadProgress;
  public onUploadBegin onUploadBegin;
}
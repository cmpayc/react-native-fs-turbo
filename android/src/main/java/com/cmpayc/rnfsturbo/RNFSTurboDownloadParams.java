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

import java.io.File;
import java.net.URL;
import java.util.Map;

public class RNFSTurboDownloadParams {
  public interface OnTaskCompleted {
    void onTaskCompleted(RNFSTurboDownloadResult res);
  }

  public interface OnDownloadBegin {
    void onDownloadBegin(int statusCode, long contentLength, Map<String, String> headers);
  }

  public interface OnDownloadProgress {
    void onDownloadProgress(long contentLength, long bytesWritten);
  }

  public URL src;
  public File dest;
  public Map<String, String> headers;
  public int progressInterval;
  public float progressDivider;
  public int readTimeout;
  public int connectionTimeout;
  public OnTaskCompleted onTaskCompleted;
  public OnDownloadBegin onDownloadBegin;
  public OnDownloadProgress onDownloadProgress;
}

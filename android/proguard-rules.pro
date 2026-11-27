# The C++ side finds these classes and their methods by name through JNI (FindClass, GetMethodID),
# so R8 must not rename or remove them.
-keep class com.cmpayc.rnfsturbo.** { *; }

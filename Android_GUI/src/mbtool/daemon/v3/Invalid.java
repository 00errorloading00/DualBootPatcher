// automatically generated, do not modify

package mbtool.daemon.v3;

import java.nio.*;
import java.lang.*;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class Invalid extends Table {
  public static Invalid getRootAsInvalid(ByteBuffer _bb) { return getRootAsInvalid(_bb, new Invalid()); }
  public static Invalid getRootAsInvalid(ByteBuffer _bb, Invalid obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__init(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public Invalid __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; return this; }


  public static void startInvalid(FlatBufferBuilder builder) { builder.startObject(0); }
  public static int endInvalid(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
};


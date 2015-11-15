// automatically generated, do not modify

package mbtool.daemon.v3;

import java.nio.*;
import java.lang.*;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class MbSwitchRomResponse extends Table {
  public static MbSwitchRomResponse getRootAsMbSwitchRomResponse(ByteBuffer _bb) { return getRootAsMbSwitchRomResponse(_bb, new MbSwitchRomResponse()); }
  public static MbSwitchRomResponse getRootAsMbSwitchRomResponse(ByteBuffer _bb, MbSwitchRomResponse obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__init(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public MbSwitchRomResponse __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; return this; }

  public boolean success() { int o = __offset(4); return o != 0 ? 0!=bb.get(o + bb_pos) : false; }
  public short result() { int o = __offset(6); return o != 0 ? bb.getShort(o + bb_pos) : 0; }

  public static int createMbSwitchRomResponse(FlatBufferBuilder builder,
      boolean success,
      short result) {
    builder.startObject(2);
    MbSwitchRomResponse.addResult(builder, result);
    MbSwitchRomResponse.addSuccess(builder, success);
    return MbSwitchRomResponse.endMbSwitchRomResponse(builder);
  }

  public static void startMbSwitchRomResponse(FlatBufferBuilder builder) { builder.startObject(2); }
  public static void addSuccess(FlatBufferBuilder builder, boolean success) { builder.addBoolean(0, success, false); }
  public static void addResult(FlatBufferBuilder builder, short result) { builder.addShort(1, result, 0); }
  public static int endMbSwitchRomResponse(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
};


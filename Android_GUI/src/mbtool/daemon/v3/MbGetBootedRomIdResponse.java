// automatically generated, do not modify

package mbtool.daemon.v3;

import java.nio.*;
import java.lang.*;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class MbGetBootedRomIdResponse extends Table {
  public static MbGetBootedRomIdResponse getRootAsMbGetBootedRomIdResponse(ByteBuffer _bb) { return getRootAsMbGetBootedRomIdResponse(_bb, new MbGetBootedRomIdResponse()); }
  public static MbGetBootedRomIdResponse getRootAsMbGetBootedRomIdResponse(ByteBuffer _bb, MbGetBootedRomIdResponse obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__init(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public MbGetBootedRomIdResponse __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; return this; }

  public String romId() { int o = __offset(4); return o != 0 ? __string(o + bb_pos) : null; }
  public ByteBuffer romIdAsByteBuffer() { return __vector_as_bytebuffer(4, 1); }

  public static int createMbGetBootedRomIdResponse(FlatBufferBuilder builder,
      int rom_id) {
    builder.startObject(1);
    MbGetBootedRomIdResponse.addRomId(builder, rom_id);
    return MbGetBootedRomIdResponse.endMbGetBootedRomIdResponse(builder);
  }

  public static void startMbGetBootedRomIdResponse(FlatBufferBuilder builder) { builder.startObject(1); }
  public static void addRomId(FlatBufferBuilder builder, int romIdOffset) { builder.addOffset(0, romIdOffset, 0); }
  public static int endMbGetBootedRomIdResponse(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
};


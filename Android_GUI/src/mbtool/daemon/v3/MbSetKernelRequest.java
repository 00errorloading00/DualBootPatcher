// automatically generated, do not modify

package mbtool.daemon.v3;

import java.nio.*;
import java.lang.*;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class MbSetKernelRequest extends Table {
  public static MbSetKernelRequest getRootAsMbSetKernelRequest(ByteBuffer _bb) { return getRootAsMbSetKernelRequest(_bb, new MbSetKernelRequest()); }
  public static MbSetKernelRequest getRootAsMbSetKernelRequest(ByteBuffer _bb, MbSetKernelRequest obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__init(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public MbSetKernelRequest __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; return this; }

  public String romId() { int o = __offset(4); return o != 0 ? __string(o + bb_pos) : null; }
  public ByteBuffer romIdAsByteBuffer() { return __vector_as_bytebuffer(4, 1); }
  public String bootBlockdev() { int o = __offset(6); return o != 0 ? __string(o + bb_pos) : null; }
  public ByteBuffer bootBlockdevAsByteBuffer() { return __vector_as_bytebuffer(6, 1); }

  public static int createMbSetKernelRequest(FlatBufferBuilder builder,
      int rom_id,
      int boot_blockdev) {
    builder.startObject(2);
    MbSetKernelRequest.addBootBlockdev(builder, boot_blockdev);
    MbSetKernelRequest.addRomId(builder, rom_id);
    return MbSetKernelRequest.endMbSetKernelRequest(builder);
  }

  public static void startMbSetKernelRequest(FlatBufferBuilder builder) { builder.startObject(2); }
  public static void addRomId(FlatBufferBuilder builder, int romIdOffset) { builder.addOffset(0, romIdOffset, 0); }
  public static void addBootBlockdev(FlatBufferBuilder builder, int bootBlockdevOffset) { builder.addOffset(1, bootBlockdevOffset, 0); }
  public static int endMbSetKernelRequest(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
};


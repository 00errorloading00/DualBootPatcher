// automatically generated, do not modify

package mbtool.daemon.v3;

import java.nio.*;
import java.lang.*;
import java.util.*;
import com.google.flatbuffers.*;

@SuppressWarnings("unused")
public final class MbGetInstalledRomsRequest extends Table {
  public static MbGetInstalledRomsRequest getRootAsMbGetInstalledRomsRequest(ByteBuffer _bb) { return getRootAsMbGetInstalledRomsRequest(_bb, new MbGetInstalledRomsRequest()); }
  public static MbGetInstalledRomsRequest getRootAsMbGetInstalledRomsRequest(ByteBuffer _bb, MbGetInstalledRomsRequest obj) { _bb.order(ByteOrder.LITTLE_ENDIAN); return (obj.__init(_bb.getInt(_bb.position()) + _bb.position(), _bb)); }
  public MbGetInstalledRomsRequest __init(int _i, ByteBuffer _bb) { bb_pos = _i; bb = _bb; return this; }


  public static void startMbGetInstalledRomsRequest(FlatBufferBuilder builder) { builder.startObject(0); }
  public static int endMbGetInstalledRomsRequest(FlatBufferBuilder builder) {
    int o = builder.endObject();
    return o;
  }
};


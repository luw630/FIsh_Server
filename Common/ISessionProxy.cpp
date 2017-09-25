#include "ISessionProxy.h"
#include "WfLog.h"
#include "WfCommonPackets.h"
NS_WF_BEGIN




int ISessionProxy::PostMsg(PacketCommand cmd,
	::google::protobuf::Message* packet,
	int32 msgParamLen,
	uint8* msgParam,
	uint16 srcType,
	uint32 srcId,
	int32 desType,
	int32 transferType,
	uint32 desId
)
{
	CProxyHead proxyHead;
	proxyHead.m_srcType = srcType;
	proxyHead.m_srcId = srcId;
	proxyHead.m_desType = desType;
	proxyHead.m_transferType = transferType;
	proxyHead.m_stTransferInfo.nDstID = desId;

	uint8 s_sendBuffer[8192];
	PacketHeader* header = (PacketHeader*)s_sendBuffer;
	header->m_Command = cmd;
	header->m_PacketParam = 0;

	int32 nSpace = 8192 - sizeof(PacketHeader);
	Stream stream((uint8*)(header + 1), nSpace);
	proxyHead.Serialize(stream);
	stream << msgParamLen;
	if (msgParamLen > 0) stream.BytesSerialize(msgParam, msgParamLen);
	nSpace -= stream.GetOffset();

	if (packet) {
		int32 size = packet->ByteSize();
		if (size > nSpace) {
			log_err("space is mall");
			return false;
		}
		else {
			bool rst = packet->SerializeToArray(stream.GetOffsetPointer(), size);
			if (!rst) {
				log_err("unknow error");
				return false;
			}
			else {
				stream.SetPointer(size);
			}
		}

		header->m_Length = sizeof(PacketHeader) + stream.GetOffset();
	}
	else {
		header->m_Length = sizeof(PacketHeader);
	}
	return SendPacket(header);
}

NS_WF_END

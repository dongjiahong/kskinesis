#ifndef _KS_KINESIS_H_
#define _KS_KINESIS_H_

#include <aws/core/Aws.h>
#include <aws/kinesis/KinesisClient.h>
#include <aws/core/utils/memory/AWSMemory.h>
#include <aws/core/utils/threading/Executor.h>
#include <aws/kinesis/model/PutRecordRequest.h>

#include "ks_timer.h"

using namespace Aws::Kinesis;
using namespace Aws::Client;
using namespace Aws::Utils;
using namespace Aws::Kinesis::Model;
using namespace std;

class KsKinesis {
public:
	KsKinesis(const Aws::String& streamName, const Aws::String& partition) :
		m_partition(partition), m_streamName(streamName) {

		// 创建客户端配置类
		ClientConfiguration clientConfiguration;
		// 设置客户端的region，不配置无法正常运行 "ap-southeast-1"
		clientConfiguration.region = m_partition;
		// 设置线程池 10 个线程
		clientConfiguration.executor = Aws::MakeShared<Aws::Utils::Threading::PooledThreadExecutor>("sample", 10);
		// 实例化客户端
		m_client = Aws::New<KinesisClient> ("sample", clientConfiguration);

		m_streamDataIterator = "";
		m_describeStreamOutcome = nullptr;
	}

	// 销毁实例
	~KsKinesis() {
		Aws::Delete(m_client);
	}

	// 查看流信息
	void KsStreamDescription();

	// 上传数据接口
	void KsStreamDataPush(const Aws::Vector<ByteBuffer>& data);

	// 拉取数据接口
	void KsStreamDataPull();
	//Aws::Vector<Record> KsStreamDataPull();
	
	// 获取log记录
	Aws::Vector<Record>& KsGetDataRecords() {
		return m_dataRecords;
	}

	// 将ByteBuffer 转换为 string
	string ByteBufferToString(const ByteBuffer &b) {
		string res("");
		for (size_t i = 0; i < b.GetLength(); ++i) {
			res += b.GetItem(i);
		}
		return res;
	}

private:
	Aws::String SetStreamDataIteratorHorizon(const Aws::String streamName, const Aws::String shardId);

	Aws::String GetStreamDataIterator() { return m_streamDataIterator; }

	void OnPutRecordsAsyncOutcomeReceived(const KinesisClient*, const PutRecordsRequest&,
			const Model::PutRecordsOutcome& outcome, const shared_ptr<const AsyncCallerContext>&);
	void OnDescribeStreamOutcomeReceived(const KinesisClient*, const Model::DescribeStreamRequest&,
			const Model::DescribeStreamOutcome& outcome, const shared_ptr<const AsyncCallerContext>&);
	void OnGetRecordsAsyncOutcomeReceived(const KinesisClient*, const Model::GetRecordsRequest&,
			const Model::GetRecordsOutcome& outcome, const shared_ptr<const AsyncCallerContext>&);


	KinesisClient *m_client;
	Aws::String m_partition;
	Aws::String m_streamName;
	Aws::Vector<Record> m_dataRecords;
	Aws::Kinesis::Model::DescribeStreamOutcome *m_describeStreamOutcome;

	Aws::String m_streamDataIterator;

};
#endif

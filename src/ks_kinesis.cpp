#include <aws/kinesis/model/PutRecordsRequest.h>
#include <aws/kinesis/model/GetRecordsRequest.h>
#include <aws/kinesis/model/DescribeStreamRequest.h>
#include <aws/kinesis/model/GetShardIteratorRequest.h>
#include <aws/kinesis/model/Record.h>
#include <aws/core/utils/Outcome.h>

#include <iostream>
#include <unistd.h>

#include "ks_kinesis.h"
#include "base.h"

using namespace std;
using namespace Aws::Client;
using namespace Aws::Utils;
using namespace Aws::Kinesis;
using namespace Aws::Kinesis::Model;

// push 数据
void KsKinesis::KsStreamDataPush(const Aws::Vector<ByteBuffer>& data) {
	PutRecordsRequest putRecordsRequest;
	putRecordsRequest.SetStreamName(m_streamName);

	for(auto& datum : data)
	{
		PutRecordsRequestEntry putRecordsRequestEntry;
		putRecordsRequestEntry.WithData(datum)
			.WithPartitionKey(m_partition);

		putRecordsRequest.AddRecords(putRecordsRequestEntry);
	}

	m_client->PutRecordsAsync(putRecordsRequest,
			std::bind(&KsKinesis::OnPutRecordsAsyncOutcomeReceived,
				this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3,
				std::placeholders::_4));
}
	
	// 获取stream描述
void KsKinesis::KsStreamDescription() {
	DescribeStreamRequest describeStreamRequest;
	describeStreamRequest.SetStreamName(m_streamName); // 必须指定流的名字，不然不能获取信息

	DescribeStreamOutcome describeStreamOutcome = m_client->DescribeStream(describeStreamRequest);

	if (describeStreamOutcome.IsSuccess()) {
		StreamDescription sd = describeStreamOutcome.GetResult().GetStreamDescription();
		cout << "streamName: "<< sd.GetStreamName() << endl;

		do {
			const Aws::Vector<Shard> shards = sd.GetShards();
			for (auto s : shards) {
				cout << "shards: " << s.GetShardId() << endl;
			}
		}while (sd.GetHasMoreShards());

		cout << "HasMoreShards:" << sd.GetHasMoreShards() << endl;
	} else {
		cout << "get describeStream err: " + describeStreamOutcome.GetError().GetMessage() << endl;
	}
}

void KsKinesis::KsStreamDataPull() {
	Aws::Vector<Record> res;
	GetRecordsRequest getRecordsRequest;
	getRecordsRequest.SetLimit(10);
	Aws::String iter = SetStreamDataIteratorHorizon(m_streamName, "ShardId-000000000002");
	if (iter.length() > 0) {
		getRecordsRequest.SetShardIterator(iter);
	} else {
		cout << "get iterator err " << endl;
		return;
	}

	while(true) {
		{
			unique_lock<mutex> locker(dataMutex);
			for (size_t i=0; i < 20; i++) {
				GetRecordsOutcome outcome = m_client->GetRecords(getRecordsRequest);
				if (outcome.IsSuccess()) {
					Aws::Vector<Record> rs= outcome.GetResult().GetRecords();
					if (rs.size()) {
						res.insert(res.end(), rs.begin(), rs.end());
					}
					cout << "---->>>>>-sizeof(res)---"<< res.size() << endl;
				} else {
					cout << "get records err : " << outcome.GetError().GetMessage() << endl;
				}
				getRecordsRequest.SetShardIterator(outcome.GetResult().GetNextShardIterator());
			}

			m_dataRecords.swap(res);	
			res.clear();
			dataReady = true; // 数据准备好了
		}

		dataCon.notify_all(); // 通知处理程序处理数据
		cout << "notify_all script" << endl;

		{
			unique_lock<mutex> locker(dataMutex);
			cout << "wait all script done" << endl;
			dataCon.wait(locker, []{
				if (runDataThreadNum == 0) {
					return dataProcess;  // 只有在所有消费线程都处理完后，才继续加载数据
				} else {
					return false; // 还有消费线程在消费
				}
			}); // 阻塞处理程序是否完成

			dataProcess = false;
			cout << "all script done" << endl;
		}
	}
}

Aws::String KsKinesis::SetStreamDataIteratorHorizon(const Aws::String streamName, const Aws::String shardId) {
	GetShardIteratorRequest getShardIteratorRequest;
	getShardIteratorRequest.SetStreamName(streamName);
	// shardID "shardId-000000000002"
	getShardIteratorRequest.SetShardId(Aws::String(shardId));
	getShardIteratorRequest.SetShardIteratorType(ShardIteratorType::TRIM_HORIZON);

	GetShardIteratorOutcome outcome = m_client->GetShardIterator(getShardIteratorRequest);
	if (outcome.IsSuccess()) {
		return outcome.GetResult().GetShardIterator();
	} else {
		cout << "get sharditerator err: " << outcome.GetError().GetMessage() << endl;
		return "";
	}
}

void KsKinesis::OnGetRecordsAsyncOutcomeReceived(const KinesisClient*, const Model::GetRecordsRequest&,
		const Model::GetRecordsOutcome& outcome, const shared_ptr<const AsyncCallerContext>&) {
	if (outcome.IsSuccess()) {
		Aws::Vector<Record > rs = outcome.GetResult().GetRecords();
		for (auto r : rs) {
			ByteBuffer b = r.GetData();
			cout << b.GetUnderlyingData() << endl;
		}
		//SetShardIterator(outcome.GetResult().GetNextShardIterator());
	} else {
		cout << "get records err : " << outcome.GetError().GetMessage() << endl;
	}
}

void KsKinesis::OnPutRecordsAsyncOutcomeReceived(const KinesisClient*, const Model::PutRecordsRequest&,
		const Model::PutRecordsOutcome& outcome, const shared_ptr<const AsyncCallerContext>&) {
	if(outcome.IsSuccess()){
		cout << "Records Put Successfully " << endl;
	}
	else {
		cout << "Put Records Failed with error " << outcome.GetError().GetMessage() << endl;
	}
}

void KsKinesis::OnDescribeStreamOutcomeReceived(const KinesisClient*, const Model::DescribeStreamRequest&,
		const Model::DescribeStreamOutcome& outcome, const shared_ptr<const AsyncCallerContext>&) {
	if (outcome.IsSuccess()) {
		cout << " stream name: " << outcome.GetResult().GetStreamDescription().GetStreamName() << endl;
	} else {
		cout << " get stream name error: " << outcome.GetError().GetMessage() << endl;
	}

}

int mainx() {
	Aws::SDKOptions options;
	Aws::InitAPI(options);

	KsKinesis ks("gift", "ap-southeast-1");

	for (int i = 0; i < 2; ++i) {
		Aws::String event1("Event #1 c++ test");
		Aws::String event2("Event #2 cxx test");

		ks.KsStreamDataPush( {
				ByteBuffer((unsigned char*)event1.c_str(), event1.length()),
				ByteBuffer((unsigned char*)event2.c_str(), event2.length())
				});
	}

	ks.KsStreamDescription();

	for (auto rs:ks.KsGetDataRecords()) {
		ByteBuffer b = rs.GetData();
		cout << b.GetUnderlyingData() << endl;
	}

	Aws::ShutdownAPI(options);
	return 0;
}

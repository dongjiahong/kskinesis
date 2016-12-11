#include <aws/kinesis/model/DescribeStreamRequest.h>
#include <aws/kinesis/model/StreamDescription.h>
#include <aws/kinesis/KinesisClient.h>
#include <aws/core/Aws.h>

using namespace Aws::Client;
using namespace Aws::Kinesis;
using namespace Aws::Kinesis::Model;

int main() {
	Aws::SDKOptions options;
	Aws::InitAPI(options);

	DescribeStreamRequest describeStreamRequest;

	std::cout << describeStreamRequest.GetStreamName() << std::endl;
	std::cout << describeStreamRequest.GetExclusiveStartShardId() << std::endl;

	Aws::ShutdownAPI(options);
	return 0;
}

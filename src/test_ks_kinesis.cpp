#include "ks_kinesis.h"
#include <chrono>
#include <string>

using namespace std;

void testByteBuffer(Aws::String str) {
	cout << "begin str: " << str << endl;
	auto x = ByteBuffer((unsigned char*)str.c_str(), str.length());
	cout << "end str: " << x.GetUnderlyingData() << endl;
}

int main() {
	Aws::SDKOptions options;
	Aws::InitAPI(options);

	KsKinesis ks("gift", "ap-southeast-1");
	ks.KsStreamDescription();

	for (auto i=0; i < 10; i++) {
		Aws::String event("{\"age\": 3}");
		cout << event << endl;
		ks.KsStreamDataPush({ ByteBuffer((unsigned char*)event.c_str(), event.length()) });
	}

	for (auto rs : ks.KsGetDataRecords() ) {
		ByteBuffer b = rs.GetData();
		cout << ks.ByteBufferToString(b) << endl;
	}
	
	//testByteBuffer("aassbb #  tsst");
	//testByteBuffer(Aws::String("aassbb"));
	

	this_thread::sleep_for(chrono::milliseconds(5000));
	Aws::ShutdownAPI(options);
	return 0;
}

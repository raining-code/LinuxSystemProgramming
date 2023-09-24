ALL:client server
server:ReactorServer.cc Server.cc TcpServer.cc Util.cc mathcalc.pb.cc
	g++ $^ -o $@ -std=c++11 -lprotobuf
client:Client.cc Util.cc mathcalc.pb.cc
	g++ $^ -o $@ -std=c++11 -lprotobuf
.PHONY:clean
clean:
	rm -rf server client
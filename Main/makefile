target = ./target
header = ./BufferMgr/headers -I./Catalog/headers -I./Qunit/headers
source = ./BufferMgr/source/*.cc ./Catalog/source/*.cc ./main.cc

$(target):$(source)
	g++ $(source) -I $(header) -o $(target)
	$(target)

exec:$(target)
	$(target)

clean:
	rm $(target)
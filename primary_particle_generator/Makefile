#all: generatePrimaryParticleData_01826
all: generatePrimaryParticleData

generatePrimaryParticleData:
	g++ generatePrimaryParticleData.cpp \
		-o generatePrimaryParticleData \
		$(shell root-config --cflags --libs) -Wall

generatePrimaryParticleData_01826:
	g++ generatePrimaryParticleData_01826.cpp \
		-o generatePrimaryParticleData_01826 \
		$(shell root-config --cflags --libs) -Wall

clean:
	rm generatePrimaryParticleData_01826
	rm generatePrimaryParticleData

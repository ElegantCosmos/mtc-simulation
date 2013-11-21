all: generatePrimaryParticleData_20 generatePrimaryParticleData

generatePrimaryParticleData:
	g++ generatePrimaryParticleData.cpp -o generatePrimaryParticleData -I/opt/clhep/clhep_2.1.0.1/include/ -L/opt/clhep/clhep_2.1.0.1/lib/ -lCLHEP -Wall

generatePrimaryParticleData_20:
	g++ generatePrimaryParticleData_20.cpp -o generatePrimaryParticleData_20 -I/opt/clhep/clhep_2.1.0.1/include/ -L/opt/clhep/clhep_2.1.0.1/lib/ -lCLHEP -Wall

clean:
	rm generatePrimaryParticleData_20
	rm generatePrimaryParticleData

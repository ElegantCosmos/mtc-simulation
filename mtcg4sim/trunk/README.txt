################################################################################

Updates made to the MTC Geant4 simulation program as of 2013/01/30.

+ Directory structure
Here are the individual files that are included in this program along with their
explanations.
mtcg4sim.cc	 - file containing program main()
run.mac	 - macro file for simulation settings
vis.mac	 - macro file for visualization settings
mtc_cad/ - directory containing CAD .stl files describing detector frame
input/	 - directory containing input file describing primary neutron and
				 	positron information for inverse-beta decay of typical reactor
				 	neutrino energy spectrum.
tools/	 - miscellaneous directory for tool such as HepRApp.jar for
				 	viewing HepRep files.
src/		 - directory containing all source files.
include/ - directory containing all header files.
mac/		 - directory containing miscellaneous macro files such as ones
				 generated using the generate_mac.sh script.

+ Using multiple CPUs.
Geant4 uses only one CPU core by default. I don't know how to setup
multi-threading capabilities, but each run can be assigned to different CPUs to
cut simulation time. A simple shell script to create multiple macros (each with
different random number generator seeds to ensure that the runs don't become
identical with each other) that can be run in parallel. This is provided in the
mac/generate_mac.sh script. So if you have 10 CPUs, you can run 10 runs in
parallel each with for example 100000 events.

+ CMake
Geant4 is moving to CMake instead of the older Make for code compilation.  CMake
seems to have benefits such as not touching the source code directory when
compiling and running a program. So we will start using CMake too. An example of
compiling and running this program with CMake is as follows:
1) Downoad trunk version of program. Let it be inside a folder called "trunk".
2) And follow this example:
mich@michpc2:~/test$ ls
trunk
mich@michpc2:~/test$ mkdir trunk-build
mich@michpc2:~/test$ cd trunk-build/
mich@michpc2:~/test/trunk-build$ cmake ../trunk
mich@michpc2:~/test/trunk-build$ make
mich@michpc2:~/test/trunk-build$ ./mtc vis.mac run.mac

+ User controls
Disregard script make_run.sh. No longer used.
Now MTCG4Sim only uses two input macro files: vis.mac and run.mac. All user
controls are done using these two files. You can run MTCG4Sim with them by doing
one of the following in the CMake build directory after successful a compile:
1) ./mtc
(Run in interactive mode. You can type "help" at prompt for details.)
2) ./mtc run.mac
(Run using simulation settings that are set in run.mac without visualization.)
3) ./mtc vis.mac run.mac
(Run using visualization settings in vis.mac along with run.mac)

+ User output
MTCG4Sim has two outputs. They are text files with specific names and output
directories that you set in the run.mac macro. One is for all particle steps of
all particles of all events in given run. This can be used for any general
analysis regarding particle properties. The other is a file which describes the
PMT readout. Note that this only has the photoelectron hit time, PMT ID number,
anode number. This can be used as a mock readout file on which you can set
your own smearing or detector response parameters to do event
reconstruction.

+ More refined geometry for PMT structure
Now the PMT geometry is more refined. Included are the glass housing, inner
vacuum, photocathode surface, dynode structure, black wrapping material, rear
PCB.

+ New process for photon hits at PMT photocathode surface
MTCG4Sim now incorporates a new process to record photon hits at the
photocathode surface. The photocathode is now a thin 20nm layer sandwiched
between the front PMT glass and inner vacuum. Photons that arrive at this
surface can undergo reflection/refraction/absorption. Photons that are absorbed
are checked against the energy dependent quantum efficiency to see if a
photoelectron is produced. Only photoelectrons that travel toward the dynode
structure are recorded as a "hit". Photons that are refracted into the vacuum
can bounce around inside of the PMT or be absorbed by the glass housing
material, etc., or may even produce a photoelectron on the way out.

+ CAD designed detector peripheral geometries
MTCG4Sim now is able to import CAD designed geometries such as the Delrin
frame/clamps structure and metal stand-off rods. Because these geometries are
complex and difficult to make in Geant4, they were imported from already-made
CAD drawings for these geometries. There are some prerequisite libraries that need to be installed to use this feature.
1) The Visualization and Computer Graphics Library (Tested with revision 4041.)
2) cadmesh (Tested with version 0.7.)
If you cannot install these or if you do not want to use this feature, set
_frame_is_placed = false, in MTCG4DetectorConstruction.cc constructor.

################################################################################

Updates made to the MTC Geant4 simulation program as of 2012/01/11.

Simulation messengers were added to this program.
The program is run by executing the make_run.sh script.
Some of the parameters in this script (such as file path) will have to be
modified to be compatible with your personal computer settings.
All simulation variables that can be used in this simulation are listed along
with their explanations in the run.mac macro file.
All visualisation options are located inside of the vis.mac macro file.

################################################################################

Contact Mich at michinar@hawaii.edu if you have any questions. --Mich 2011/08/26


############### Author ####################

Name: Rui Yang 
SFUID:301130789
mail: ruiy@sfu.ca

############### Instruction ###############

1. On first run the default mesh is empty.

2. In the ".SMF File" panel, input the file path in the "Filename:" text area and click "OPEN", it should be able to open the file and display the mesh object unless the file path is not correct or the file format is not .smf. 

For example, if the file horse.smf is in the sub level directory "meshobject", the correct path should be
meshobject/horse.smf

If the horse.smf is in the parent level directory, the correct path should be
../horse.smf

If horse.smf is in the same directory with the executable, the correct path should be
horse.smf

A small test file sphere.smf is included in the package. 

Click the "OPEN" with incorrect file path will restore the view that displays nothing. However, if there is at least one mesh object rendered before, clicking the "SAVE" will save the current mesh to "saved.smf" under the same directory of executable no matter the mesh is displaying or not. Open another valid SMF file will replace the old mesh with new one. 

!!!CAUTION!!! After you save the current mesh to "saved.smf", please back up it by copying or changing the name. Otherwise the next SAVE will overwrite the current file.

4. In light panel, there are one controlable light and one fixed light. The intensity, RGB and rotation setting is for controlable light only. You can only enable or disable the fixed light.

5. The rest of the interaction control is self-explanation.

6. When you switch from one mesh to another, you might need to adjust the XY translation and also zooming to get an appropriate view.  

################ END ######################

########## Notes for Mesh Decimation ######

1. On the first load of a mesh it might take a long time to display the mesh because now it will first compute and store the distince edge. The time depends on the size of the mesh; the larger the longer. For example, small-horse.smf with size 370 kB will take about 10 sec. horse.smf with size 1.3 MB will take about 120 sec.

2. The decimation time of each run is also related to the size of mesh and the edge numbers to collapse each time. For example it will take 15 sec to collapse 2000 edges from 16800 edges to 14800 edges. Or it will take 66 sec to collapse 2000 edges from 59547 edges to 57546 edges in horse.smf. As long as the total edge size goes down, the decimation time is shorter.

3. The algorithm is based on Quadric error metric and Multiple choice random set. The default randomize set size is 8. It is defined at mcaq.cpp line 64 
#define RANDOM_SET_SIZE 8

4. This program passes the test for horse.smf however the program runnability and performance are not guaranteed for the mesh larger than 1.3 MB. Issue might rise as the memory leaking. The mesh decimation correctness should be fine though. More tests over extremely large mesh should be done.

5. A small timer is used in command window for displaying each run's information.

Thank you for your time.


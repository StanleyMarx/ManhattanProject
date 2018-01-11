# MAP DATA
By JB

```
$ make run
```
This will compile and run the program ```openFile```. <br/>

## Movements of the robots and writing them in a file
The script should open (or create) a text file ```pos.txt``` and put the (fake) coordinates of the robot in it.<br/>
These coordinates will be the ones the robot sends to the server as it explores the map.

## Generating the map once the robot is done exploring
At this point the robot should have explored the map (and therefore find the frontiers and corners of the map).<br/>
So the program looks at all of its past coordinates to deduce the possible corners of the map (to build a rectangular map).<br/>
It then generates all of the points in that area and checks whether the robots went on those coordinates.<br/>
To be used with the BT connection, we'll need to add the ```send_map_message``` function with the right color, for each of the map data point.

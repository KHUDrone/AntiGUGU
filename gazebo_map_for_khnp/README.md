# Gazebo map for KHNP's KVRC competition
+ Season 1 - [map](https://youtu.be/6oXx2bvzU9Y), [github](https://github.com/Woojin-Seol/KVRC2021)
+ Season 2 - [map](https://youtu.be/usKW3UG9wpc), [github](https://github.com/Woojin-Seol/KVRC2022)

## How to use
+ Clone the git
~~~shell
$ git clone git@github.com:engcang/gazebo_map_for_khnp
~~~

+ Add Gazebo Path
~~~shell
$ cd gazebo_map_for_khnp

for season 1
$ echo "export GAZEBO_MODEL_PATH=$GAZEBO_MODEL_PATH:$(pwd)/season1/refracted_corridor_map:$(pwd)/season1/rough_terrain_map:$(pwd)/season1/stair_map:$(pwd)/season1/qr_codes:$(pwd)/season1/manipulator_map:$(pwd)/season1/disturbance_map:$(pwd)/season1/common" >> ~/.bashrc

for season 2
$ echo "export GAZEBO_MODEL_PATH=$GAZEBO_MODEL_PATH:$(pwd)/season2" >> ~/.bashrc

$ . ~/.bashrc
~~~

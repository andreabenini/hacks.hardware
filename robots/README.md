# Omni/Multi purpose
- **GoogleX moonshot robots**  
  Kinda cool stuff here, multipurpose robots, wheeled bots with basic movements and 6 deg freedom arms. A lot of algebra and spacial recognition in human complex environments.
  These bots are aimed toward solving daily tasks: sorting, cleaning, navigating, .... Not an easy task but they're doing real good. Lots of impressive feats
  (some storytelling), inside mechanics, good inner details on basic macro areas from their videos (massive still frames to learn from them).  
  - https://everydayrobots.com/
  - https://x.company/projects/everyday-robots/
- **Yandex’s delivery robot**  
  The story behind the creation of Yandex’s delivery robots. This is a really nice project
  description, much like some sort of storytelling/blog series. It's not too technical, there are
  no in depth information, there's no code but as a maker I really appreciate the whole
  project description, missed steps, project iterations, trial and error stages and so on.
  The bot idea is heavily borrowed from Amazon Scout, it has the same shape/size/mechanics
  and basic ideas but in a truly Russian style (sturdy frame, minimal components, less flashy
  but reliable).  
  Navigation software from their autonomous cars biggest brothers, I'm really impressed by them:
  - [The story behind yandex bots](https://medium.com/yandex-self-driving-car/the-story-behind-the-creation-of-yandexs-delivery-robot-e07017940589),
  storytelling, no tech details but really fun
- **HP SitePrint**  
  HP robotic layout robot solution. Quick, accurate and easy construction site layouts from a small robot.
  Nice idea, sturdy mechanics, interesting robot layout but poor execution in a real construction site (never seen a cleanest building site).
  Solid mechanic in a compact shaped layout, definitively I can grab some valid ideas from it.  
  https://www.hp.com/us-en/printers/site-print/layout-robot.html
- **OpenBot**  
  Smartphone + cheap open hardware = https://openbot.org (https://github.com/isl-org/OpenBot)
- **Guardian Bot**  
  Cool little robot, ESP32 powered, used as a simple security guard. Some basic concepts are intersting and its compact design is really awesome.
  It's connected to Home Assistant too:  
  https://github.com/PricelessToolkit/Smart-Home-Security-Guardian-Bot
- **Small spy car**
  Not my favorite but source code is good and the idea is nice, I'll surely get rid of these cheap motors and the small lipo battery for something bigger
  but more reliable.  
  https://www.youtube.com/watch?v=gmqgAcnKeFU&ab_channel=TheWrench
- **HeRo 2.0: A Low-Cost Robot for Swarm Robotics Research**  
  https://arxiv.org/pdf/2202.12391.pdf
- **Mobile robot with a 4DOF arm and an onboard camera**  
  Arduino project with a mixup of boards: RPi Pico, ESP32/CAM, VNH3ASP30 motor driver  
  https://blog.arduino.cc/2023/07/18/this-remote-controlled-highly-mobile-robot-features-a-4dof-arm-and-an-onboard-camera/
- **Watako Lab**  
  The only problem here is using google translator across the whole site but projects are definitively awesome  
  http://watako-lab.com/
- **Doly**
  Open robot project based on Raspberry PI CM4, simple and outstanding chassis and mechanics  
  https://github.com/robotdoly/
- **3D Dog Robot**  
  Multiple Servo Motor Control Board. Arduino+ESP8266  
  https://www.instructables.com/Explore-Simple-3D-Dog-Robot-and-Multiple-Servo-Mot/?utm_source=newsletter&utm_medium=email
- **MiniSkidi - 3D Printed RC SkidSteer**  
  Small and cutie bobcat like mini robot. Robust and nice hw design, really simple sw part based 
  on arduino. Controllable from your mobile phone with a simple, generic and free app: Dabble. Available for Android and iOS.
  - Presentation video https://www.youtube.com/watch?v=3jgQ9fDi1nY
  - Software repository (arduino based) https://github.com/ProfBoots/MiniSkidi/tree/main, compatible with
    mobile phone Dabble client
  - Bluetooth client controller software. Using Dabble for it
    - Intro https://ai.thestempedia.com/docs/dabble-app/getting-started-with-dabble/
    - Dabble sources https://github.com/STEMpedia/Dabble
    - Android application https://play.google.com/store/apps/details?id=io.dabbleapp
    - iOS application https://apps.apple.com/us/app/dabble-bluetooth-controller/id1472734455
- **3D Printed Fish Feeder RC Boat With Controller**  
RC Boat using classic 2 motors scheme, ESP32C3 and minimal specs. ESP-NOW protocol and complete design for the
transmitter part too, well documented and source code available.  
https://www.instructables.com/3D-Printed-Fish-Feeder-RC-Boat-With-Controller/


# Gamepads
- some kind of ESP32 gamepad, seems to be a viable choice for controlling robots and other appliances. It's unfortunately based
  on the Arduino stack so no direct optimizations with ESP-IDF libraries are possible withouth Arduino libs:  
  https://github.com/lemmingDev/ESP32-BLE-Gamepad


# Desktop companions
- **Dodge - ESP32 Cam Based Tripod**
  This funny device is a  sort of companion with a cam, building process is really well documented and mechanics it's solid too. It's motion might be maybe
  limited but the mechanics is excellent.  
  https://www.instructables.com/Dodge-ESP32-Cam-Based-Tripod/?utm_source=newsletter&utm_medium=email

# Walkers
- **Build your own robotic cat**<br>
  Awesome open source project with reliable body and quite good design, Arduino controlled board could be replaced with a
  pca9685 compatible device and few additional circuits. Body movements are near perfect if you exclude cheap chinese 9g
  plastic servos.<br>
  BTW, impressive feats: https://www.raspberrypi.org/blog/robotic-cat-petoi-nybble/
- **Quadruped Robot Tsuki**<br>
  Do not follow this bot only. Lingkang Zhang seems to be a really talented guy, his creations are awesome and there's a lot of material, mostly related on quadruped robots but his research papers are useful as a whole. Nice to see him at http://zlethic.com/


# Rocker Bogie designs
- Stair climbing robot  
  https://www.thingiverse.com/thing:5363032  
  DTU Robocup contender, see also Thingiverse Martian Robot design (link in its description) where this one got design ideas
- Rocker Bogie Mechanism  
  https://www.slideshare.net/HaniroothChandraseka/rocker-bogie-mechanism-design-and-fabrication
  Basic explaination but there's an important thing here: all formulas for calculating the length of the links, tilt base, wheels and so on.
- ExoMy: A Low Cost 3D Printed Rover.  
  https://github.com/esa-prl/ExoMy/wiki  
  Kinda ESA version for babies
- DIY Mars Perseverance Rover Replica – Arduino based Project  
  https://howtomechatronics.com/projects/diy-mars-perseverance-rover-replica-with-arduino/  
  Probably one of the best and accurate project I've ever seen. Mechanics part is really well detailed and impressive,
  STL cad files are well engineered and suitable for generic robots too, it's not just for a bare replica but easily
  adaptable to a generic rover bogie robot.
- Sawppy the rover  
  https://newscrewdriver.com/category/projects/sawppy-the-rover/  
  This is good, a lot of variations from original Nasa project, aims to be practical and not just a bare replica
- Overall design on Research Gate  
  https://www.researchgate.net/publication/313403109_Design_of_Rocker_Bogie_Mechanism
- Bunch of designs, each one has good points and ideas:  
  - https://github.com/Charlie5DH/Rocker-Bogie-Robot
  - https://www.thingiverse.com/thing:2414954
  - https://www.thingiverse.com/thing:682828
  - https://www.thingiverse.com/thing:3576552
  - https://www.google.com/search?q=rocker+bogie+3d+print&tbm=isch&ved=2ahUKEwik7YPHqYv6AhVug_0HHRw6B1IQ2-cCegQIABAA&oq=rocker+bogie+3d+print&gs_lcp=CgNpbWcQDDoGCAAQHhAIOggIABCxAxCDAToECAAQAzoFCAAQsQM6BAgAEEM6BwgAELEDEEM6CAgAEIAEELEDOgUIABCABFC8DFjqWmDIfmgAcAB4AIABSogBzwmSAQIyMJgBAKABAaoBC2d3cy13aXotaW1nwAEB&sclient=img&ei=hRYdY-SWCO6G9u8PnPSckAU&bih=1508&biw=2844#imgrc=c84oqVa9Yj5u5M
- Rocker Bogie differential explained with Lego  
  http://alicesastroinfo.com/2012/07/mars-rover-rocker-bogie-differential/  
  Love robots, images, math, examples and Lego ? This covers you all


# Motors
- **Tips for selecting DC motors for your mobile robot**  
  Self explanatory title showing how to correctly use proper Math in a real scenario. Definitively the best resource
  for it, it takes a while to read and understand the whole article but it's a really good read.  
  https://www.servomagazine.com/magazine/article/tips_for_selecting_dc_motors_for_your_mobile_robot
- **10 Steps to Choosing the Right Motors for Your Robotic Project**  
  Some math and useful information for choosing the right motor for your projects.  
  https://medium.com/husarion-blog/10-steps-to-choosing-the-right-motors-for-your-robotic-project-bf5c4b997407
- **L298N, Arduino code & schematics**  
  How to control DC motors using Arduino and the L298N motor driver.  
  https://howtomechatronics.com/tutorials/arduino/arduino-dc-motor-control-tutorial-l298n-pwm-h-bridge/  
  https://dronebotworkshop.com/dc-motors-l298n-h-bridge/  
  https://www.instructables.com/Control-A-DC-Mototr-Using-H-Bridge/
- **tb6612fng motor driver, a L298N replacement**  
  https://www.hackster.io/news/tb6612fng-motor-driver-better-than-the-l298n-7499a7574e63
- RMF/RPM motor calculator  
  https://www.societyofrobots.com/RMF_calculator.shtml

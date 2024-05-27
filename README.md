# Hi, I'm Will Wang, this is intro page for the EZmap full-stack software! 
<p>Software Enginner at <a href="https://www.engineering.utoronto.ca/">University of Toronto</a></br></p>

### A little more about me and the algorithm...  

```javascript
const EZmap = {
  name: "EZmap",
  code: [C++, C, Python, Shell Bash],
  tools: [Git, JSON, GTK+],
  take_away: "Give it a try"
}
```
## Disclaimer Regarding Why I will NOT Post the Full Code of This Project!

The source code available in this repository is provided for educational and demonstration purposes only. It is not intended for use in actual production environments without proper authorization and adaptation. For academic integrity purposes, I will NOT post the full code of this project since there are individuals stuck in academic integrity problems with UofT due to copying the code of this project

### Academic Integrity

Please be aware that using this code in academic settings without proper citation or in a manner that violates academic integrity rules could lead to serious consequences including disciplinary actions by educational institutions. If you are a student or an educator, it is your responsibility to ensure that using this code complies with your institution's academic integrity policies.

### Code Use and Modification

The code segments provided here are intended as a reference to illustrate concepts discussed in the EZmap project. They may be modified or removed at any time without notice, depending on the feedback from the community, changes in project direction, or concerns related to privacy or security.

### Contributing to the Project

Contributors are encouraged to discuss potential changes via issues and pull requests to ensure that the project remains a valuable and safe resource for everyone involved. When contributing, please ensure that your modifications adhere to the original purpose of educational enhancement and do not infringe on any copyrights or intellectual property rights.

By using, copying, or contributing to this code, you agree to respect the academic and intellectual property guidelines as outlined. Thank you for your cooperation and understanding.



## Key Features
- **Hybrid Displaying**: Combines driving and public transportation options in one seamless route.
- **Double Paths Algorithm**: Utilizes both Dijkstra's and A* algorithms to find the fastest and most accurate path.
- **Traffic Signal Integration**: Provides real-time traffic light data to minimize stops and reduce travel time.
- **Visual Subway Line Suggestions**: Displays subway lines visually for easy navigation planning.
- **Night Mode**: Reduces screen brightness automatically under low light conditions to reduce eye strain.
- **Green Light Optimal Speed Advisory (GLOSA)**: Advises on the optimal speed to maintain to hit green lights, enhancing fuel efficiency and reducing stoppage time.
- **Machine Learning User Behavior(on-going)**: Predicts and suggests routes based on individual commuting patterns.

## Overview
Maps have become an integral part of our daily lives, and people rely on them for a wide range of purposes. Whether it's finding directions, discovering new places, or planning a trip, maps are essential tools. The ability to customize and personalize maps to fit individual needs has become an essential part of our lives, helping us make informed decisions and navigate the world around us.

However, the current navigation apps do not cater to their unique needs. This gap in the current navigation apps is that they offer only limited options for transportation modes, such as walking, driving, or public transportation; they don't have combined modes for two or more ways of transportation. 

These separated modes do not cater to the hybrid commuting approach of the majority of commuters in the Greater Toronto Area. 

![Copy of Team 066 OP2 Presentation (1)](https://github.com/Liyourong/EZmap_Next_Gen_Commuter_Map/assets/105031962/5ab44faa-6169-439f-b8bb-87b2359da3c1)


## What is this GIS?
This GIS visualizes integrated geographic information from multiple public APIs(Such as OSM API). It is equipped with a visually appealing user interface using GTK+ tools to display street maps, public transportation, and natural geographic features of major cities such as Beijing, Toronto, and Tokyo. Users can find their route by entering the start and destination point information, and the program will calculate the optimal route using Dijkstra and A* algorithm. This GIS has also left public interfaces for online real-time information such as live traffic and weather

<img width="1000" alt="Picture of Beijing" src="https://user-images.githubusercontent.com/105031962/228465134-08c8f6b4-e035-46dd-b5b4-78de284c8cf2.png">



## How it looks?
As discussed, this GIS relies on two sources: the pre-loaded APIs, such as OpenStreetMap API; the other way is to use libcurl to fetch additional information online. This means that our GIS could display any information in the public API.

For example, let's take a tour of the Harry Potter world:

<img width="1080" alt="Kings Cross" src="https://user-images.githubusercontent.com/105031962/228466020-7f0fd31f-83d2-4d29-963d-869d08a51d08.png">

This image is the Kings Cross Station. As you can see, our GIS displays the subway stations, subway lines, and traffic signals for demonstration. 

Other than that, as you might have noticed, the map also load natural features from the APIs and visualize them on the screen! How about taking a tour of the forbidden city?

<img width="1086" alt="Picture Of Forbidden City" src="https://user-images.githubusercontent.com/105031962/228466848-0e2356e7-0505-49e9-8f34-c1184897a1cb.png">

Or a nigh version view at GTA?
![Copy of Team 066 OP2 Presentation (15)](https://github.com/Liyourong/EZmap_Next_Gen_Commuter_Map/assets/105031962/a6f47652-440e-45e2-8f40-7fe2dc657c42)



## Mission
Therefore, a map app that offers a wider range of transportation options, including hybrid modes, is urgently needed. This way, commuters can choose the most efficient and convenient mode that suits their unique needs and preferences. That's our mission

![Copy of Team 066 OP2 Presentation (3)](https://github.com/Liyourong/EZmap_Next_Gen_Commuter_Map/assets/105031962/b703484e-3a24-4fa3-b323-0d9f4ba061ff)

## Real-life Applications
Transitioning from a relaxed schedule to a more structured and demanding work routine is never easy. The hassle often starts before any work begins. To get a reasonable transportation estimate, the user must search the driving route between home and the subway/go station and the route from the subway station to work on another transportation map.

Designing a commute route might be simple in the afternoon when you just had a fight with your manager, but it’s much more complicated in the morning due to sleep inertia.

Let's imagine you are a commuter who travels from Thornhill Club in North York to Union Station in downtown Toronto every weekday morning. The usual approach involves driving to the Finch TTC station and taking the subway to the destination. However, when using Google Maps for navigation, the available options are limited to driving or public transportation. This makes it challenging for commuters to get a clear view of the entire route and estimate the travel time accurately. They may have to search twice or more to find their ideal route.

Our map app has been designed to address this problem by offering a hybrid transportation mode. When the user inputs their starting and ending points, the app provides a route that includes both driving and public transportation options. The driving route is displayed in a distinctive color, making it easy for users to identify and follow. This way, commuters can quickly and easily navigate the complex transportation system and accurately estimate travel time. With our map app, commuters can find their route in one search and get to their destination efficiently and conveniently.
![Copy of Team 066 OP2 Presentation (4)](https://github.com/Liyourong/EZmap_Next_Gen_Commuter_Map/assets/105031962/69623850-384c-4214-8766-272c1d2ccf25)
![Copy of Team 066 OP2 Presentation (5)](https://github.com/Liyourong/EZmap_Next_Gen_Commuter_Map/assets/105031962/b01a0fb4-09c2-48c0-b9bb-8f4a9ea6e210)

## Double Paths Algorithm
Here is the algorithm we implemented called the double paths algorithm. The "Double Path" algorithm is a combination of A* and Dijkstra algorithm. The A* algorithm makes decisions based on the heuristic functions and returns results much faster than the Dijkstra algorithm. Therefore it is used to create and display an estimate of the optimal route as it returns results very efficiently. This is a technique we use to quickly generate a solution that is close to the optimal solution. 

After the A* algorithm generates the rough estimate, we then use Dijkstra's algorithm to find the exact, accurate result. Dijkstra's algorithm is a very good choice when finding the shortest path between two points. It works by exploring all possible paths from the starting node to the ending node, keeping track of the shortest path found so far. Once Dijkstra's algorithm finds the result, it swaps out the A* ‘s result, and the more precise route will be displayed.

With the double pathing route, we could return a fair result in one click, and a more accurate one in the following 3-5 seconds. This algorithm is used since Dijkstra usually returns results in a longer time,  and from our research, users typically expect to see a response in 1 second. This enhances the mapping software's responsiveness without sacrificing the quality of the route.
![Copy of Team 066 OP2 Presentation (7)](https://github.com/Liyourong/EZmap_Next_Gen_Commuter_Map/assets/105031962/d7137ece-2eb1-4c57-b155-7c0a7735e156)

![Copy of Team 066 OP2 Presentation (8)](https://github.com/Liyourong/EZmap_Next_Gen_Commuter_Map/assets/105031962/c1e7f08d-9868-48c3-b471-a9bdd85eed4d)

![Copy of Team 066 OP2 Presentation (9)](https://github.com/Liyourong/EZmap_Next_Gen_Commuter_Map/assets/105031962/56130614-c88d-482c-8499-cc2828ecaa1c)

![Copy of Team 066 OP2 Presentation (10)](https://github.com/Liyourong/EZmap_Next_Gen_Commuter_Map/assets/105031962/76e9a928-83bc-4375-b2ac-13f7071186bb)


## Future Pitch
The green light optimal speed advisory gives real time advice on the optimal speed the user should drive at. This optimal speed allows the driver to hit on as many green lights as possible and avoid any unnecessary red light. For example, here we calculated that the optimal speed is 50 kilometers per hour, so this is the speed that the user can drive without waiting for any red lights and therefore save time and optimize the travel time.  

By collecting the user's typical driving speed, the software can analyze and estimate the expected travel time more accurately when providing route suggestions to the user.

The next one is about machine learning user’s common behaviours. This predicts the user’s preferred routes by analyzing the user's historical driving routes. For example, the user may not like to drive on a highway, so each time the user might pick the alternative route when there is a highway in the suggestion. The software will analyze this and ensure that the highway will only appear if it is the shortest path or it is the only path. If the highway is present in the suggestion, the software will provide an alternative that does not include a highway for the user to pick whenever possible. Based on news or statistics online, the software can also analyze things such as the correlation between a certain road and weather to avoid any potential risks. For example, the system analyzes that street A is dangerous to drive on when snowing since the road is very steep and there were several traffic incidents happened before, so the software will avoid using road A on snowy days and use alternatives. This can greatly increase user’s satisfaction level and also bring up usability.

Here is a chart that shows three of the most popular mapping software in the market. All three mapping softwares do not include any machine learning for travel time optimization as well as the GLOSA algorithm. The GLOSA algorithm can be crucial. For example, route A is theoretically the fastest route, but if the user did not drive at an optimal speed and need to wait for several red lights, it can dramatically increase the travel time.

So our software is very competitive in the market as all the three maps are not providing optimized routes using machine learning or the GLOSA algorithm.

Our map focused on the hybrid transportation group and provided them with a high usability and high responsive mapping software. With the future pitch feature implemented, our unique mapping software will be highly competitive in the market.

![Copy of Team 066 OP2 Presentation (11)](https://github.com/Liyourong/EZmap_Next_Gen_Commuter_Map/assets/105031962/91100abc-9036-4aee-a5d8-cc0048ab4e1b)

![Copy of Team 066 OP2 Presentation (12)](https://github.com/Liyourong/EZmap_Next_Gen_Commuter_Map/assets/105031962/2e985831-dd05-4f69-a68a-165d0c1964c4)

![Copy of Team 066 OP2 Presentation (13)](https://github.com/Liyourong/EZmap_Next_Gen_Commuter_Map/assets/105031962/8b8e25d6-34d7-4452-9931-36c5b352df58)

![Copy of Team 066 OP2 Presentation (14)](https://github.com/Liyourong/EZmap_Next_Gen_Commuter_Map/assets/105031962/32515f10-f049-421a-b98c-3006dc8db176)












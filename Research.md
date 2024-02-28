# Research

This folder just includes a bunch of links to pages or libraries I used as sources when implementing this framework.

Want to leave a big shout out to [Lazy Foo' Productions](https://lazyfoo.net/tutorials/SDL/index.php). They created possible the best guide for getting started with SDL around. It's based on SDL2, but is still a treasure trove of important information that everyone planning on using SDL should read through.

### Collisions

A lot of this I took from my own C#/MonoGame library, [Precisamento.MonoGame](https://github.com/Precisamento/Precisamento.MonoGame), which itself is a rewrite of the physics from [Nez](https://github.com/prime31/Nez)

I've unfortunately lost a lot of the other pages and StackOverflow posts I've used, which were mostly used to implement the missing collision methods/types.

### Exceptions

These are implemented using a modified version of the [exceptions4c](https://github.com/guillermocalvo/exceptions4c) library. The modifications are so that they can use the right allocator and also so the public symbols can be properly exported on Windows (stupid MSVC DLLs).

### Fonts

The main font engine is [SDL_ttf](https://github.com/libsdl-org/SDL_ttf). The TTF font renderer is based on [SpriteFont](https://github.com/MonoGame/MonoGame/blob/develop/MonoGame.Framework/Graphics/SpriteFont.cs) from MonoGame and the [SDL_FontCache](https://github.com/grimfang4/SDL_FontCache) library. The latter currently doesn't support SDL3, which is why the custom renderer was implemented.

### Graphics Primitives

There are a lot of inspirations for this implementation. The functionality for drawing points, lines, and rects is built in to SDL3.

I had used [sdl-gpu](https://github.com/grimfang4/sdl-gpu) with older versions of SDL, but that was no longer an option with SDL3. SDL3 supports drawing primitives via triangles though, which is a vast improvement on the previous system.

I looked through a few different libraries for inspiration, but didn't have much luck with full implementations. These include [SDL2_gfx](https://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/index.html), [SDL_prims](https://www.piumarta.com/software/sdlprims/), and [libSGE](https://github.com/flibitijibibo/libSGE/tree/master) for SDL specific libraries. Some vague inspiration comes from the [Primitives2D](https://github.com/DoogeJ/MonoGame.Primitives2D) MonoGame library as well.

#### Polygons

None of the solutions above struck my fancy for filled polygon drawing, so a custom implementation was implemented. Some digging on ways to draw these brought me to the [Wikipedia page for Polygon Triangulation](https://en.wikipedia.org/wiki/Polygon_triangulation). I figured out how to implement triangulation for convex polygons based on the description on the page, and decided to search around the web for Ear Clipping implementations. This turned out to be far more difficult than expected, since there aren't many articles/libraries containing concrete implementations out in the wild. Here are some of the articles I read on the subject:
1. https://www.geometrictools.com/Documentation/TriangulationByEarClipping.pdf (*)
1. https://swaminathanj.github.io/cg/PolygonTriangulation.html (*)
1. https://digitalscholarship.unlv.edu/cgi/viewcontent.cgi?article=2314&context=thesesdissertations
1. https://www.diva-portal.org/smash/get/diva2:330344/FULLTEXT02.pdf
1. https://en.wikipedia.org/wiki/Two_ears_theorem
1. https://citeseerx.ist.psu.edu/document?repid=rep1&type=pdf&doi=81f137deaff65cbb0d4e7c6dbc5548b47080087a
1. https://nils-olovsson.se/articles/ear_clipping_triangulation/ (*)

(*) - Good, concrete information.

Unfortunately, I was too stupid to follow any of the above. Luckily, there was a [video implementing the algorithm in MonoGame](https://www.youtube.com/watch?v=QAdfkylpYwc) that was trivial to convert to C. This guy is my hero.

#### Circles

There are a bunch of circle drawing routines in the libraries listed above. For circle outlines, I've based the implementation off of the one from [GeeksForGeeks](https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm/). I also checked out these couple of articles [1](https://uomustansiriyah.edu.iq/media/lectures/12/12_2020_06_26!11_47_57_PM.pdf), [2](https://www.javatpoint.com/computer-graphics-bresenhams-circle-algorithm).

### Math

Most of the math stuff comes from functions I've collected/implemented over the years. Most come from [MonoGame](https://github.com/MonoGame/MonoGame) and [MonoGame.Extended](https://github.com/craftworkgames/MonoGame.Extended).

#### Matrix

The 2D matrix code comes from MonoGame.Extended, and I double checked the implementation using the following articles on Affine Transformations:

* https://en.wikipedia.org/wiki/Affine_transformation
* https://people.computing.clemson.edu/~dhouse/courses/401/notes/affines-matrices.pdf
* https://math.stackexchange.com/questions/2093314/rotation-matrix-of-rotation-around-a-point-other-than-the-origin
* http://dirsig.cis.rit.edu/docs/new/affine.html

### Input

I think the original implementation for this came from a GameMaker tutorial on their legacy forums. Either way, it's been changed a lot over the years, with each iteration making it slightly more usable (IMO). This final version comes from my C# implementation in [Precisamento.MonoGame](https://github.com/Precisamento/Precisamento.MonoGame).

### Timers

The stopwatch is based on one defined in the [lazyfoo SDL tutorials](https://lazyfoo.net/tutorials/SDL/25_capping_frame_rate/index.php), and the timer is a heavily altered version of the [GameTimer](https://github.com/craftworkgames/MonoGame.Extended/blob/develop/src/cs/MonoGame.Extended/Timers/GameTimer.cs) class in MonoGame.Extended.

### Garbage Collector

I decided to use a Garbage Collector so that I can be lazy about memory management. All data structures that perform allocations have corresponding free functions, but the option to forego them is there. The implementation I've used is the [Boehm-Demers-Weiser Garbage Collector](https://github.com/ivmai/bdwgc) because it's well tested, is multi-threaded, and is super easy to implement.

### Entity-Component-System

I wrote a C port of the C# library [DefaultEcs](https://github.com/Doraku/DefaultEcs) called [MystEcs](https://github.com/mystborn/MystEcs). The port was originally based off of an older version of DefaultEcs, so the current version has diverged from some of its changes (and it was never a 100% faithful port to begin with).

### Data Structures

I've created and maintain a library called [GenericDataStructures](https://github.com/mystborn/GenericDataStructures) for generating generic data structures in C similar to C# generics / C++ templates. I use it in most of my C projects, including this.

### Strings

I've created a string implementation for C that includes the Small String Optimization called [sso_string](https://github.com/mystborn/sso_string). Similar to GenericDataStructures, I use it in most of my C projects.

### C

Kind of a silly inclusion, but if you're looking for the best docs on the C standard library, head over to [cppreference.com](https://en.cppreference.com/w/c/header).

## Glossary

### Libraries
* [SDL3](https://wiki.libsdl.org/SDL3/FrontPage)
* [Precisamento.MonoGame](https://github.com/Precisamento/Precisamento.MonoGame)
* [sso_string](https://github.com/mystborn/sso_string)
* [GenericDataStructures](https://github.com/mystborn/GenericDataStructures)
* [MystEcs](https://github.com/mystborn/MystEcs)
* [MonoGame](https://github.com/MonoGame/MonoGame)
* [MonoGame.Extended](https://github.com/craftworkgames/MonoGame.Extended)
* [Nez](https://github.com/prime31/Nez)
* [exceptions4c](https://github.com/guillermocalvo/exceptions4c)
* [DefaultEcs](https://github.com/Doraku/DefaultEcs)
* [SDL_FontCache](https://github.com/grimfang4/SDL_FontCache)
* [SDL_ttf](https://github.com/libsdl-org/SDL_ttf)
* [sdl-gpu](https://github.com/grimfang4/sdl-gpu)
* [SDL2_gfx](https://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/index.html)
* [bdwgc](https://github.com/ivmai/bdwgc)



### Webpages

* https://en.wikipedia.org/wiki/Polygon_triangulation
* https://www.geometrictools.com/Documentation/TriangulationByEarClipping.pdf
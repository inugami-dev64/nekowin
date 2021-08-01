# Usage

In order to start using nekowin an API must be initialised. This can be done by using `neko_InitAPI()` function and
the API instance can be deinitialised by using `neko_DeinitAPI()` Other relevant functions to consider using
are `neko_NewWindow()` which can be used to create a new window along with `neko_DestroyWindow()` 
that is used to destroy the given window instance.  


## OpenGL example

### Creating the window

OpenGL could be considered as one of the reasons nekowin library was created in the first place. It has platform independant
bindings for creating OpenGL contexts and loading OpenGL functions using glad. This tutorial is not an OpenGL tutorial, but instead
a brief guide for using nekowin as OpenGL rendering context. Before creating any OpenGL context windows
we need to know what parameters this window should have. We need to know the initial width, height and the name of the window.
Additionally we can also specify hints for the window, which describe how the window should behave and for what purpose it was
created in the first place. 

Let's create an application, where user enters the width and height of the window to stdin. For this we need to write this code snippet:  
```C++
int width, height;
std::cout << "Enter window width: ";
std::cin >> width;
std::cout << "Enter window height: ";
std::cin >> height;
```

Before creating a window, an API instance must be created. After that initialisation we are free to create new windows:  
```C++
neko_InitAPI();
neko_Window win = neko_NewWindow(width, height, NEKO_HINT_API_OPENGL | NEKO_HINT_RESIZEABLE, "GLTest");
```

First two parameters of `neko_NewWindow()` specify width and height of the window, third parameter describes window hints that
can be used and the final parameter specifies the window title.  
List of window hints are following:  
* `NEKO_HINT_API_OPENGL` - specify that the window will be used as an OpenGL context
* `NEKO_HINT_API_VULKAN` - specify that the window will be used as a Vulkan surface
* `NEKO_HINT_FULL_SCREEN` - specify that the window should be in full screen mode, ignoring the given size parameters
* `NEKO_HINT_FIXED_SIZE` - specify that the window should have fixed size
* `NEKO_HINT_RESIZEABLE` - specify that the window should be resizeable

NOTE: When using `NEKO_HINT_FULL_SCREEN` or resizing in `NEKO_HINT_RESIZEABLE` mode you should keep in mind to call `glViewport` as well to
update the viewport for current rendering context.


### Loading OpenGL functions

After creating a rendering context we can proceed to loading OpenGL functions. For this we can use a macro called `neko_LoadGL()`. 
The macro definition is literally the same as `gladLoadGL` which returns a status code after initialisation attempt. We can use
this status code the determine if the OpenGL library loading was successful or not:  

```C++
int status = neko_LoadGL();
if(!status) {
    std::cerr << "Failed to load OpenGL functions" << std::endl;
    std::exit(-1);
}
```

When OpenGL functions are loaded we can proceed to creating shader programs, buffer handles, vertex data and / or whatever
OpenGL related procedures might be necessary for the current use case.


### Run loop

Firstly we can use `neko_IsRunning()` to determine if the window has not recieved termination signals. Secondly there are some certain 
things we need to do in every iteration cycle such as updating `glViewport`, getting input information about certain keystrokes and updating 
the window itself.  

Our main loop code would look something like that:  
```C++
while(neko_IsRunning()) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    neko_GetWindowSize(win, &x, &y);
    glViewport(0, 0, x, y);

    // Submit OpenGL draw calls
    ...

    if(neko_FindKeyStatus(NEKO_KEY_F, NEKO_INPUT_EVENT_TYPE_ACTIVE))
        std::cout << "You pressed F" << std::endl;
    else if(neko_FindKeyStatus(NEKO_KEY_F, NEKO_INPUT_EVENT_TYPE_RELEASED))
        std::cout << "You released F" << std::endl;

    neko_UpdateWindow(win);

    // Check for any errors during the frame generation
    err_check("glDrawElements");
}
```

As you might see we firstly retrieve information about the current window size using `neko_GetWindowSize()` function,
then we check if the F key is either active or released and if it is, print appropriate message about it and finally we update 
the window instance itself.

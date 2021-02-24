# 3D-Renderer
CPU based renderer/software rasterizer from scratch. SDL2 is used for input handling.  
Types of diffuse shading includes Flat shading, Gouraud shading and Phong shading.  

TBD: texture mapping, probably abstract UI and Camera

<img src="3D Renderer/images/application.png">

## Dependencies
<ul>
  <li><a href="https://github.com/Tyyppi77/imgui_sdl">imgui_sdl</a> (included in this repo)</li>  
  <li><a href="https://github.com/ocornut/imgui">imgui</a> (included in this repo)</li>  
  <li><a href="https://github.com/tinyobjloader/tinyobjloader">tinyobjloader</a> (included in this repo)</li>  
  <li><a href="https://www.libsdl.org/">SDL</a> (NOT included in this repo, simply download SDL2.0 and put it in desktop to use this project.)</li>  
</ul>


   

## Features  
### Backface culling
![culling](https://media.giphy.com/media/XnzGzm2Z0fngWABdeu/giphy.gif)  

### Ambient lighting
![culling](https://media.giphy.com/media/jyVYb2JfIiWFmQpnn3/giphy.gif)  

### Flat shading
![flat shading](https://media.giphy.com/media/LnGicmDbDdRfQ0PXO3/giphy.gif)  
  
### Gouraud shading  
![gouraud](https://media.giphy.com/media/knUtumgXfeUb8K3G1L/giphy.gif)  
  
### Point light
![point light](https://media.giphy.com/media/oZsnM9ulz8tVF46Jhf/giphy.gif)  

### Camera
![camera](https://media.giphy.com/media/kT4xHDkF1O5RAE8mFL/giphy.gif)  

### Phong Illumation Model (gouraud shading version)
![Phong Illumination Model](https://media.giphy.com/media/RlaeNoANQeAYzCNUwz/giphy.gif)

### Phong Illumation Model (Phong shading version)
![Phong Illumination Model](https://media.giphy.com/media/0ezNDQTYwnPbLdOO3V/giphy.gif)

### clipping (against z-plane) 
![clipping](https://media.giphy.com/media/XENJ19jfaOJDdhAUr8/giphy.gif)

### vertex normal visualization
![vertex normal visualization](https://media.giphy.com/media/b0U2WqtB0VoKSKwmzD/giphy.gif)


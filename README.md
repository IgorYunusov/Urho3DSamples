![Urho3D logo](https://raw.githubusercontent.com/urho3d/Urho3D/master/bin/Data/Textures/LogoLarge.png)

#Urho3D

**Urho3D** is a free lightweight, cross-platform 2D and 3D game engine implemented in C++ and released under the MIT license. Greatly inspired by OGRE and Horde3D.

Main website: [http://urho3d.github.io/](http://urho3d.github.io/)

##License
Licensed under the MIT license, see [License.txt](https://github.com/urho3d/Urho3D/blob/master/License.txt) for details.



##My Urho3D Samples
###01_AttributeInspector
look at Sample::Start() for the implementation
![Sample1](https://raw.githubusercontent.com/scorvi/Urho3DSamples/master/screenshots/01_AttributeInspector.png)
###02_OpenVGRenderer
look at Sample::CreateSVGSprite(const String& file) for the implementation

![Sample2](https://raw.githubusercontent.com/scorvi/Urho3DSamples/master/screenshots/02_OpenVGRenderer.png)

###03_NanoSVGRendering

look at NanoGUI::Render(bool resetRenderTargets /*= true*/) for the implenentaion

note: it uses the nanovgs native opengl rendering pipeline 

note: only NanoGUI::Render is at the moment implentet the other sunctions do nothing .... 

todo: create an uhro3d rendering context for nanovg

![Sample3](https://raw.githubusercontent.com/scorvi/Urho3DSamples/master/screenshots/03_NanoSVGRendering.png)

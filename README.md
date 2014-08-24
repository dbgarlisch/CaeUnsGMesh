# caeplugin-GMesh
A Pointwise CAE plugin that exports a grid in the Gmsh File Format.

![Gmsh][Logo]

Gmsh is a 3D finite element grid generator with a build-in CAD engine and post-processor. 
Its design goal is to provide a fast, light and user-friendly meshing tool with 
parametric input and advanced visualization capabilities. Gmsh is built around four 
modules: geometry, mesh, solver and post-processing. The specification of any input 
to these modules is done either interactively using the graphical user interface or 
in ASCII text files using Gmsh's own scripting language.

For more information see:
* [Gmsh website][Web]

## Building the Plugin
To build the Gmsh plugin you must integrate this source code into your local PluginSDK 
installation by following these steps.

* Download and install the [Pointwise Plugin SDK][SDKdownload].
* Configure and validate the SDK following the [SDK's instructions][SDKdocs].
* Create a Gmsh plugin project using the mkplugin script: `mkplugin -uns -cpp GMesh`
* Replace the project's generated files with the files from this repository.

This plugin was created using version 1.0 R7 of the Pointwise CAE Plugin SDK.


## Disclaimer
Plugins are freely provided. They are not supported products of
Pointwise, Inc. Some plugins have been written and contributed by third
parties outside of Pointwise's control.

TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, POINTWISE DISCLAIMS
ALL WARRANTIES, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED
TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE, WITH REGARD TO THESE SCRIPTS. TO THE MAXIMUM EXTENT PERMITTED
BY APPLICABLE LAW, IN NO EVENT SHALL POINTWISE BE LIABLE TO ANY PARTY
FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS
INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR
INABILITY TO USE THESE SCRIPTS EVEN IF POINTWISE HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES AND REGARDLESS OF THE FAULT OR NEGLIGENCE OF
POINTWISE.

[Logo]: https://raw.github.com/dbgarlisch/CaeUnsGMesh/master/logo_gmsh.png  "Gmsh Logo"
[Web]: http://geuz.org/gmsh/
[SDKdocs]: http://www.pointwise.com/plugins
[SDKdownload]: http://www.pointwise.com/plugins/#sdk_downloads

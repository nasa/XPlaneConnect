X-Plane Connect (XPC) Toolbox

Description
The X-Plane Connect (XPC) Toolbox facilitates communication with X-Plane. This toolbox allows for the real-time manipulation of X-Plane variables. Possible applications include active control of an XPlane simulation, flight visualization, recording states during a flight, or interacting with a mission over UDP.

Architecture
XPC includes a plugin (xpcPlugin) which is to be copied into the X-Plane Plugin Directory ([X-Plane Directory]/Resources/Plugin/), and the xpcScripts-A series of functions for communication with X-Plane.

xpcPlugin (Directory: xpcPlugin/)

xpcScripts
    C: (Directory: C/src/, Example: C/xpcExample/)
    MATLAB: (Directory: Matlab/+XPlaneConnect/, Example: MATLAB/xpcExample/)
    java: (future-Not complete yet): (Directory: java/src/, Example: java/xpcExample/)
    python: (future-Not complete yet): (Directory: python/xpc/, Example: python/xpcExample/)
    
Instructions:
    1. Purchase/Install X-Plane
    2. Copy the X-Plane Plugin folder (xpcPlugin/XPlaneConnect) to the plugin directory ([X-Plane Directory]/Resources/plugins/)
    3. Write code using the xpcScrips to manipulate X-Plane
    
Capabilities:
    Set Aircraft Position (own or other aircraft): Use sendPOSI()
    Control Aircraft (own or other aircraft): Use sendCTRL()
    Set any internal X-Plane dataref: Use sendDREF()
    Get the value of any X-Plane dataref: Use requestDREF()
    Pause Simulation: Use pauseSim()
    
Compatability:
    Windows: 
        Tested on Windows Vista and Windows 7
    Mac OSX
        Tested on OS X 10.8-10.9
    X-Plane
        Tested with X-Plane 9 & 10

Notices:

Copyright ©2013-2014 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved.

Disclaimers

No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."

Waiver and Indemnity:  RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
X-Plane API
Copyright (c) 2008, Sandy Barbour and Ben Supnik  All rights reserved.
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Neither the names of the authors nor that of X-Plane or Laminar Research
      may be used to endorse or promote products derived from this software
      without specific prior written permission from the authors or
      Laminar Research, respectively.
 
X-Plane API SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    
Contributors:
    CT: Chris Teubert (christopher.a.teubert@nasa.gov)

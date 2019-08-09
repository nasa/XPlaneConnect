# X-Plane Connect
The X-Plane Connect (XPC) Toolbox is an open source research tool used to
interact with the commercial flight simulator software X-Plane. XPC allows users
to control aircraft and receive state information from aircraft simulated in
X-Plane using functions written in C, C++, Java, MATLAB, or Python in real time over the
network. This research tool has been used to visualize flight paths, test control
algorithms, simulate an active airspace, or generate out-the-window visuals for
in-house flight simulation software. Possible applications include active control
of an XPlane simulation, flight visualization, recording states during a flight,
or interacting with a mission over UDP.

### Architecture
XPC includes an X-Plane plugin (xpcPlugin) and clients written in several
languages that interact with the plugin.

#### Quick Start
To get started using X-Plane Connect, do the following.

1. Purchase and install X-Plane 9, 10 or 11.
2. Download the `XPlaneConnect.zip` file from the latest release on the [releases](https://github.com/nasa/XPlaneConnect/releases) page.
3. Copy the contents of the .zip archive to the plugin directory (`[X-Plane Directory]/Resources/plugins/`)
4. Write some code using one of the clients to manipulate X-Plane data.

Each client is located in a top-level directory of the repository named for the
client's language. The client directories generally include a `src` folder
containing the client source code, and an `Examples` folder containing sample
code demonstrating how to use the client.

#### Additional Information
For detailed information about XPC and how to use the XPC clients, refer to the
[XPC Wiki](https://github.com/nasa/XPlaneConnect/wiki).

#### Capabilities
The XPC Toolbox allows the user to manipulate the internal state of X-Plane by
reading and setting DataRefs, a complete list of which can be found on the
[X-Plane SDK wiki](http://www.xsquawkbox.net/xpsdk/docs/DataRefs.html).

In addition, several convenience functions are provided, which allow the user to
efficiently execute common commands. These functions include the ability to set
the position and control surfaces of both player and multiplayer aircraft. In
addition, the pause function allows users to easily pause and un-pause X-Plane's
physics simulation engine.

### Compatibility
XPC has been tested with the following software versions:
* Windows: Vista, 7, & 8
* Mac OSX: 10.8-10.14
* Linux: Tested on Red Hat Enterprise Linux Workstation release 6.6
* X-Plane: 9, 10 & 11

### Contributing
All contributions are welcome! If you are having problems with the plugin, please
open an issue on GitHub or email [Chris Teubert](mailto:christopher.a.teubert@nasa.gov).
If you would like to contribute directly, please feel free to open a pull request
against the "develop" branch. Pull requests will be evaluated and integrated into
the next official release.


### Notices
Copyright ©2013-2018 United States Government as represented by the Administrator
of the National Aeronautics and Space Administration.  All Rights Reserved.

No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY
KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY
WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM
INFRINGEMENT, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY
WARRANTY THAT DOCUMENTATION, IF PROVIDED, WILL CONFORM TO THE SUBJECT SOFTWARE.
THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT
AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE,
SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT
SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES
REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND
DISTRIBUTES IT "AS IS."

Waiver and Indemnity:  RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE
UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY
PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS IN ANY
LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES ARISING FROM SUCH USE,
INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, RECIPIENT'S USE
OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED
STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR
RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE REMEDY FOR ANY SUCH
MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.

#### X-Plane API
Copyright (c) 2008, Sandy Barbour and Ben Supnik  All rights reserved.
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Neither the names of the authors nor that of X-Plane or Laminar Research may
  be used to endorse or promote products derived from this software without
  specific prior written permission from the authors or Laminar Research,
  respectively.

X-Plane API SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

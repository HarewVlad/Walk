# Walk  
# How to compile  
Source:  
cl /Zi /MTd /D _DEBUG main.cpp // cl main.cpp  
  
Shaders:  
fxc /T vs_5_1 /Fo vs.cso vs.hlsl  
fxc /T ps_5_1 /Fo ps.cso ps.hlsl

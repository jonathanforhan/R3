#version 460

layout (location = 0) out uint f_EntityID;

layout (push_constant, std140) uniform FragmentPushConstants {
layout(offset = 64)
    uint c_EntityID;
};

void main() {
    f_EntityID = c_EntityID;    
}

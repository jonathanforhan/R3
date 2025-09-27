#version 460

layout (location = 0) out uint f_EntityID;

layout (push_constant) uniform PushConstants {
    mat4 c_Model;
    uint c_EntityID;
};

void main() {
    f_EntityID = c_EntityID;    
}

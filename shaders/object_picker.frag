#version 460

layout (location = 0) flat in uint v_ID;

layout (binding = 1) buffer ShaderStorageBufferObject {
	uint s_SelectedID;
};

layout (location = 0) out float f_Color;

void main() {
	s_SelectedID = v_ID;
	f_Color = float(s_SelectedID);
}

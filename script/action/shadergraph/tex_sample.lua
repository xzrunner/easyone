-- create node TextureSample

require "moon.sg"

local node = moon.sg.new_node("sg_tex_sample")
if node then
	node:set_pos(moon.mouse.get_position())
end
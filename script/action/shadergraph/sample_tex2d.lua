-- create node SampleTex2D

require "moon.sg"

local node = moon.sg.new_node("sg_sample_tex2d")
if node then
	node:set_pos(moon.mouse.get_position())
end
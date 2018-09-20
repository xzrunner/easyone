-- create node GradientNoise

require "moon.sg"

local node = moon.sg.new_node("sg_gradient_noise")
if node then
	node:set_pos(moon.mouse.get_position())
end
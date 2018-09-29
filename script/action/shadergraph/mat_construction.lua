-- create node MatrixConstruction

require "moon.sg"

local node = moon.sg.new_node("sg_mat_construction")
if node then
	node:set_pos(moon.mouse.get_position())
end
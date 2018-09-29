-- create node MatrixDeterminant

require "moon.sg"

local node = moon.sg.new_node("sg_mat_determinant")
if node then
	node:set_pos(moon.mouse.get_position())
end
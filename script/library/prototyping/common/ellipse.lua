function init(self)
	require 'gui'
	local ellipse = g_object(require 'gui.widgets.ellipse')

	ellipse.o_width  = 170
	ellipse.o_height = 170
	ellipse.o_color  = { 255, 255, 255 }

	self.v_widget = ellipse	

	local w, h = self.v_widget.o_width, self.v_widget.o_height
	self.v_node:set_size(w, h)
end

function update(self)
	self.v_widget:update()
end

function draw(self)
	local x, y = self.v_node:get_pos()
	self.v_widget:draw(x, y)
end
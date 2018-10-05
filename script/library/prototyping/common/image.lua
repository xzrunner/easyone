function init(self)
	require 'gui'
	local img = g_object(require 'gui.widgets.image')

	local img_path = moon.gui.open_file_dlg()
	if img_path then
		img.o_img = moon.graphics.new_image(img_path)
	end	

	img.o_width  = img.o_img:get_width()
	img.o_height = img.o_img:get_height()

	self.v_widget = img

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
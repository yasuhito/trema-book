# encoding: utf-8

class HelloTrema < Controller
  def switch_ready(datapath_id)
    info 'Hello %#x!' % datapath_id
  end
end

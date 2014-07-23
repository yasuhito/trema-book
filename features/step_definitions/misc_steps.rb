# encoding: utf-8

When(/^wait until "([^"]*)" is up$/) do |process|
  nloop = 0
  pid_file = File.join(Trema.pid, "#{process}.pid")
  loop do
    nloop += 1
    fail 'Timeout' if nloop > 100 # FIXME
    break if FileTest.exists?(pid_file) && ps_entry_of(process)
    sleep 0.1
  end
  sleep 1  # FIXME
end

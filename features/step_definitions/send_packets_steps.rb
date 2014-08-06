# encoding: utf-8

When(/^I send (\d+) packets from (.+) to (.+)$/) do |n_packets, host_a, host_b|
  step "I run `trema send_packets --source #{host_a} --dest #{host_b} --n_pkts #{n_packets}`"
end

When(/^I send 1 packet from (.+) to (.+)$/) do |host_a, host_b|
  step "I send 1 packets from #{host_a} to #{host_b}"
end

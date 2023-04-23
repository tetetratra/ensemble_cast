# frozen_string_literal: true

require "bundler/gem_tasks"
require "rake/extensiontask"

task build: :compile

Rake::ExtensionTask.new("ensemble_cast") do |ext|
  ext.lib_dir = "lib/ensemble_cast"
end

task default: %i[clobber compile]

# frozen_string_literal: true

require_relative "lib/ensemble_cast/version"

Gem::Specification.new do |spec|
  spec.name = "ensemble_cast"
  spec.version = EnsembleCast::VERSION
  spec.authors = ["tetetratra"]
  spec.email = ["44367208+tetetratra@users.noreply.github.com"]

  spec.summary = "Evaluate block in first argument"
  spec.description = <<~EOS
    Evaluate block in first argument.

    You can use `Module#ensemble(method_name)` method.

    When you call `ensemble(method_name)`, block passed to `method_name` will be evaluated in the context of the first argument of the block.
  EOS
  spec.homepage = "https://github.com/tetetratra/ensemble_cast"
  spec.license = "MIT"
  spec.required_ruby_version = ">= 3.1.0"

  spec.metadata["homepage_uri"] = spec.homepage
  spec.metadata["source_code_uri"] = "https://github.com/tetetratra/ensemble_cast"

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  spec.files = Dir.chdir(File.expand_path(__dir__)) do
    `git ls-files -z`.split("\x0").reject do |f|
      (f == __FILE__) || f.match(%r{\A(?:(?:test|spec|features)/|\.(?:git|travis|circleci)|appveyor)})
    end
  end
  spec.bindir = "exe"
  spec.executables = spec.files.grep(%r{\Aexe/}) { |f| File.basename(f) }
  spec.require_paths = ["lib"]
  spec.extensions = ["ext/ensemble_cast/extconf.rb"]

  # Uncomment to register a new dependency of your gem
  # spec.add_dependency "example-gem", "~> 1.0"

  # For more information and examples about making a new gem, check out our
  # guide at: https://bundler.io/guides/creating_gem.html
end

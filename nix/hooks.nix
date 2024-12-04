{pkgs}: let
  pyrun = name: pkgs.python310.interpreter + " " + name;
in {
  alejandra.enable = true;
  black.enable = true;
  trim-trailing-whitespace.enable = true;

  commit-name = {
    enable = true;
    name = "check commit name";
    stages = ["commit-msg"];
    entry = pyrun ../scripts/check_commit_msg.py;
  };

  check-include-order = {
    enable = true;
    name = "check include order";
    entry = pyrun ../scripts/check_include_order.py;
  };
}

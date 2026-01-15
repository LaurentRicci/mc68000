# Debugging on wsl
the launch.vs.json should have an entry like the following
```
    {
      "type": "cppgdb",
      "name": "simple (simple\\simple)",
      "project": "CMakeLists.txt",
      "projectTarget": "simple (simple\\simple)",
      "comment": "Learn how to configure WSL debugging. For more info, see http://aka.ms/vslinuxdebug",
      "debuggerConfiguration": "gdb",
      "args": [
        "/home/laurent/code/mc68000/asm/examples/tinybasic.bin"
      ],
      "env": {}
    }
```

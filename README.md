# Introduction 
TODO: Give a short introduction of your project. Let this section explain the objectives or the motivation behind this project. 

# Getting Started
If you are running on windows, make sure that the boost library is installed in \code\lib\boost. if a specific version is in place like \code\lib\boost_1_82_0 you can do
```
push-location C:\code\lib
New-Item -Name boost -ItemType SymbolicLink -Value .\boost_1_82_0\
```
If you are running on linux the boost library should be present in /usr/include/boost. If you have a specific version insatlled you can create a link by doing:
```
cd /usr/include
sudo ln -s boost_1_84_0/ boost
```

# Build and Test
To create the makefile just run the following command:
```
cmake .
```
To build the code just do
```
cmake --build .
```
To run the tests
```
coretest/coretest -p
```


# Contribute
TODO: Explain how other users and developers can contribute to make your code better. 

If you want to learn more about creating good readme files then refer the following [guidelines](https://docs.microsoft.com/en-us/azure/devops/repos/git/create-a-readme?view=azure-devops). You can also seek inspiration from the below readme files:
- [ASP.NET Core](https://github.com/aspnet/Home)
- [Visual Studio Code](https://github.com/Microsoft/vscode)
- [Chakra Core](https://github.com/Microsoft/ChakraCore)
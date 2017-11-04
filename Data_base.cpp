#include "stdafx.h"
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Sqlite3>
#include <string>

namespace dbo = Wt::Dbo;

　
　
enum class Role {
	Visitor = 0,
	Admin = 1,
	Employee = 42
};

class User {
public:
	std::string name;
	std::string password;
	Role        role;

	User()
	{
		name = " ";
		password = " ";
		role = Role::Visitor;
	}

	template<class Action>
	void persist(Action& a)
	{
		dbo::field(a, name, "name");
		dbo::field(a, password, "password");
		dbo::field(a, role, "role");
	}
};

void run()
{
	// Setup a session.
	// provides access to our database objects.
	auto sqlite3 = std::make_unique<dbo::backend::Sqlite3>("test.db");
	//sqlite3->setProperty("show-queries", "true");
	dbo::Session session;
	session.setConnection(std::move(sqlite3));
	
	//create table from class user
	session.mapClass<User>("user");
	try
	{
		session.createTables();
	}
	catch (...)
	{

	}
	{
		
		dbo::Transaction transaction(session);

		auto user = std::make_unique<User>();
		user->name = "jay";
		user->password = "Hush";
		user->role = Role::Visitor;
		// adds an object to the database.
		dbo::ptr<User> userPtr = session.add(std::move(user));
	}

	/*****
	Querying objects
	*****/

	{
		dbo::Transaction transaction(session);
		dbo::ptr<User> jay = session.find<User>().where("name = ?").bind("Jay");
		std:: cout << std::endl;
		try {
			dbo::ptr<User> jay2 = session.query< dbo::ptr<User> >
				("select u from user u").where("name = ?").bind("Jay");
		}
		catch (int e)
		{
		
		}

	}
	//when queries have multiple user with same name
	{
	dbo::Transaction transaction(session); 
		int count = session.query<int>("select count(1) from user")
		.where("name = ?").bind("Joe");
	}

	{
		dbo::Transaction transaction(session);
		typedef dbo::collection< dbo::ptr<User> > Users;
		Users users = session.find<User>();
		std::cerr << "We have " << users.size() << " users:" << std::endl;
		for (Users::const_iterator i = users.begin(); i != users.end(); ++i)
			std::cerr << " user " << (*i)->name << std::endl;
	}

	/*****
	Updating objects
	*****/

	{
		dbo::Transaction transaction(session);
		dbo::ptr<User> jay = session.find<User>().where("name = ?").bind("jay");
		jay.modify()->password = "public";
	}

	{
		dbo::Transaction transaction(session);
		dbo::ptr<User> jay = session.find<User>().where("name = ?").bind("jay");
		if (jay)
	   jay.remove();
	}

	{
		dbo::Transaction transaction(session);
		dbo::ptr<User> silly = session.add(std::unique_ptr<User>{new User()});
		silly.modify()->name = "Silly";
		silly.remove();
	}

}

int main()
{
	run();
	std::cin.get();
};

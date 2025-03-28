/* register_types.cpp */

#include "register_types.h"

#include "bullet.h"
#include "bullet_server.h"
#include "bullet_server_relay.h"
#include "bullet_spawner.h"

#include "resource/bullet_path.h"
#include "resource/simple_bullet_path.h"
#include "resource/bezier_bullet_path.h"
#include "resource/force_bullet_path.h"
#include "resource/bullet_texture.h"

#include "core/config/engine.h"
#include "core/object/class_db.h"

BulletServerRelay *_bullet_server_relay = nullptr;

void initialize_qurobullet_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	_bullet_server_relay = memnew(BulletServerRelay);

	GDREGISTER_CLASS(Bullet);
	GDREGISTER_CLASS(BulletServer);
	GDREGISTER_CLASS(BulletServerRelay);
	GDREGISTER_CLASS(BulletSpawner);

	GDREGISTER_ABSTRACT_CLASS(BulletPath);
	GDREGISTER_CLASS(SimpleBulletPath);
	GDREGISTER_CLASS(BezierBulletPath);
	GDREGISTER_CLASS(ForceBulletPath);
	
	GDREGISTER_CLASS(BulletTexture);

	Engine *engine = Engine::get_singleton();
	if (engine->has_singleton("BulletServerRelay")) {
		return;
	}

	engine->add_singleton(Engine::Singleton("BulletServerRelay", _bullet_server_relay));
}

void uninitialize_qurobullet_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	Engine *engine = Engine::get_singleton();
	if (engine->has_singleton("BulletServerRelay")) {
		engine->remove_singleton("BulletServerRelay");
	}

	if (_bullet_server_relay) {
		memdelete(_bullet_server_relay);
	}
}

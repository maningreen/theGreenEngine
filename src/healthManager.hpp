#ifndef healthM_h
#define healthM_h boobies

#include "barManager.hpp"

class HealthManager : public BarManager {
  private:
  public:
    float health;
    float maxHealth;

    HealthManager(float maxHealth, BarManager b);
    HealthManager(float maxHealth, float health, BarManager b);
    ~HealthManager();

    float getHealth();
    void setHealth(float h);
    void applyDamage(float d);

    bool isDead();

    float getMaxHealth();
    void setMaxHealth(float m);

    void Process(float delta) override;
};

#endif

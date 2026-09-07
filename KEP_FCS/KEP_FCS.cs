/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
using FCS_extended;
using HarmonyLib;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace KEP_FCS
{
    public enum DialogActionEnum_extended
    {
        DA_DECREASE_FACTION_RANK = 21000,
    }

    public class KepPlugin : IPlugin
    {
        public int Init(Assembly assembly)
        {
            Harmony harmony = new Harmony("KEP_FCS");
            harmony.PatchAll();
            Console.WriteLine("KEP plugin loaded.");
            return 0;
        }

        [HarmonyPatch]
        public static class Conversation_Constructor_Patch
        {
            static System.Reflection.MethodBase TargetMethod()
            {
                var conversationType = AccessTools.TypeByName("forgotten_construction_set.conversation");
                var itemTypeInfo = AccessTools.TypeByName("forgotten_construction_set.GameData+Item");
                var navigationTypeInfo = AccessTools.TypeByName("forgotten_construction_set.navigation");
                Type[] types = new Type[] { itemTypeInfo, navigationTypeInfo };
                return AccessTools.Constructor(conversationType, types);
            }

            static void Postfix(object __instance)
            {
                var PossibleEffects = AccessTools.Field("forgotten_construction_set.conversation:PossibleEffects").GetValue(__instance) as System.Windows.Forms.ComboBox;
                string[] names = Enum.GetNames(typeof(DialogActionEnum_extended));
                foreach (var item in names)
                {
                    PossibleEffects.Items.Add(item);
                }
            }
        }

        [HarmonyPatch("forgotten_construction_set.conversation", "InitializeComponent")]
        public static class Conversation_InitializeComponent_Patch
        {
            [HarmonyPostfix]
            static void Postfix(object __instance)
            {
                var effectValue = AccessTools.Field("forgotten_construction_set.conversation:effectValue").GetValue(__instance) as System.Windows.Forms.NumericUpDown;
                effectValue.Maximum = 10000000;
                effectValue.Minimum = -10000000;
            }
        }

        [HarmonyPatch("forgotten_construction_set.dialog.ConditionControl", "InitializeComponent")]
        public static class ConditionControl_InitializeComponent_Patch
        {
            [HarmonyPostfix]
            static void Postfix(object __instance)
            {
                var conditionsValue = AccessTools.Field("forgotten_construction_set.dialog.ConditionControl:conditionsValue").GetValue(__instance) as System.Windows.Forms.NumericUpDown;
                conditionsValue.Maximum = 10000000;
                conditionsValue.Minimum = -10000000;
            }
        }
    }
}

import React, { useEffect, useState, useRef } from "react";
import {
  View,
  Text,
  StyleSheet,
  Animated,
  ScrollView,
  ImageBackground,
} from "react-native";
import database from "@react-native-firebase/database";

const Card = ({ label, value }: any) => {
  const fadeAnim = useRef(new Animated.Value(1)).current;
  const prevValue = useRef(value);

  useEffect(() => {
    if (prevValue.current !== value) {
      triggerBlink();
      prevValue.current = value;
    }
  }, [value]);

  const triggerBlink = () => {
    fadeAnim.setValue(0.5);
    Animated.sequence([
      Animated.timing(fadeAnim, {
        toValue: 1,
        duration: 300,
        useNativeDriver: true,
      }),
      Animated.timing(fadeAnim, {
        toValue: 0.5,
        duration: 300,
        useNativeDriver: true,
      }),
      Animated.timing(fadeAnim, {
        toValue: 1,
        duration: 300,
        useNativeDriver: true,
      }),
    ]).start();
  };

  return (
    <Animated.View style={[styles.card, { opacity: fadeAnim }]}> 
      <Text style={styles.label}>{label}</Text>
      <Text style={styles.value}>{value}</Text>
    </Animated.View>
  );
};

const SensorScreen = () => {
  const [data, setData] = useState<any>({});//controller

  useEffect(() => {
    const ref = database().ref("controller/"); // Firebase path
    const listener = ref.on("value", (snapshot) => {
      if (snapshot.exists()) {
        console.log(snapshot.val());
        setData(snapshot.val());
      }
    });

    return () => ref.off("value", listener);
  }, []);

  return (
    <ImageBackground
      source={require("@/assets/images/bg.png")}
      resizeMode="cover"
      style={{ flex: 1, height: "100%", width: "100%" }}
    >
      <Text style={styles.header}>Automation with Flex</Text>
      <ScrollView contentContainerStyle={styles.container}>
        <Card label="Emergency" value={data?.devices?.emergency} />
        <Card label="Fan" value={data?.devices?.fan} />
        <Card label="Light" value={data?.devices?.light} />
        <Card label="Alcohol Detected" value={data?.sensors?.alcohol_detected?"Yes":"No"} />
        <Card label="Humidity" value={data?.sensors?.humidity} />
        <Card label="MQ3" value={data?.sensors?.mq3} />
        <Card label="Temperature" value={data?.sensors?.temperature} />
        
      </ScrollView>
      
    </ImageBackground>
  );
};

export default SensorScreen;

const styles = StyleSheet.create({
  container: {
    flexGrow: 1,
    justifyContent: "space-around",
    alignItems: "center",
    flexDirection: "row",
    flexWrap: "wrap",
    paddingVertical: 20,
  },
  header: {
    fontSize: 24,
    fontWeight: "bold",
    marginBottom: 20,
    marginTop: 60,
    textAlign: "center",
    color: "#fff",
  },
  card: {
    width: "45%",
    height: 150,
    padding: 20,
    borderRadius: 15,
    backgroundColor: "#ffffff5c",
    elevation: 5,
    shadowColor: "#000",
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.25,
    shadowRadius: 3.84,
    marginBottom: 15,
    alignItems: "center",
  },
  label: {
    fontSize: 16,
    fontWeight: "600",
    marginBottom: 8,
  },
  value: {
    fontSize: 30,
    color: "#000000",
    fontWeight: "700",
  },
  tableContainer: {
    width: "90%",
    marginTop: 30,
    padding: 15,
    borderRadius: 15,
    backgroundColor: "#ffffffa0",
  },
  tableTitle: {
    fontSize: 20,
    fontWeight: "700",
    marginBottom: 10,
    textAlign: "center",
  },
  tableRow: {
    flexDirection: "row",
    justifyContent: "space-between",
    paddingVertical: 6,
    borderBottomWidth: 1,
    borderBottomColor: "#ccc",
  },
  tableLabel: {
    fontSize: 16,
    fontWeight: "600",
  },
  tableValue: {
    fontSize: 16,
    fontWeight: "400",
    color: "#333",
  },
});
